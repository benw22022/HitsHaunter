#include "StripModule.h"

SCTModule::SCTModule(double xpos, double ypos, double zpos, int layer, double rotation, bool flip, std::string name, double width, double length, int nstrips, double stereo_angle) :
    m_xpos{xpos}, m_ypos{ypos}, m_zpos{zpos}, m_layer{layer}, m_width{width}, m_length{length}, m_nstrips{nstrips}, m_stereo_angle{stereo_angle}, m_rotation{rotation}, m_name{name}
    {
        if (flip){
            m_stereo_angle *= -1;
        }
        
        this->initStripEdges();
    }


void SCTModule::initStripEdges()
{
    Vector2D vertical_unit_vec{Vec2{0, 1}, Vec2{m_xpos, m_ypos}};

    for (const auto& dx : linspace(-m_width/2, m_width/2, m_nstrips+1))
    {
        m_top_strip_edges.push_back(vertical_unit_vec.translate(Vec2{dx, -m_length/2}).rotate(m_rotation, Vec2{m_xpos, m_ypos}));
        m_bottom_strip_edges.push_back(vertical_unit_vec.translate(Vec2{dx, -m_length/2}).rotate(m_rotation+m_stereo_angle, Vec2{m_xpos, m_ypos}));
    }
}

bool SCTModule::checkHitModuleOverlap(const Hit& hit, bool debug) const
{   
    double length = std::max(m_width/2, m_length/2);
    double width = std::min(m_width/2, m_length/2);

    // Define two rectangles for top and bottom of the module
    std::vector<std::pair<double, double>> unit_rect_corners{{-width, -length}, {width, -length}, {width, length}, {-width, length}};
    //*                                                         bottom left         top left         top right        bottom right
    std::vector<std::pair<double, double>> top_rect_corners{};
    std::vector<std::pair<double, double>> bottom_rect_corners{};
    
    // Rotate the rectangles 
    for (const auto& corner : unit_rect_corners)
    {   
        top_rect_corners.push_back(rotate_point(corner.first, corner.second, 0, 0, m_rotation));
        bottom_rect_corners.push_back(rotate_point(corner.first, corner.second, 0, 0, m_rotation+m_stereo_angle));
    }
    
    std::pair<double, double> hit_centred{hit.x-m_xpos, hit.y-m_ypos};

    bool inside_top_rect = point_in_rectangle(Vec2(hit_centred.first, hit_centred.second), top_rect_corners);
    bool inside_bottom_rect = point_in_rectangle(Vec2(hit_centred.first, hit_centred.second), bottom_rect_corners);

    
    if (debug)
    {   std::cout << std::endl << "---------------------" << std::endl;
        std::cout << "Hit " << hit << std::endl;
        std::cout << "Top rect = {";
        for (const auto& coord: top_rect_corners)
        {
            std::cout << "(" << coord.first << ", " << coord.second << "), ";
        }
        std::cout << "}" << std::endl;
        
        std::cout << "Bottom rect = {";
        for (const auto& coord: bottom_rect_corners)
        {
            std::cout << "(" << coord.first << ", " << coord.second << "), ";
        }
        std::cout << "}" << std::endl;


        std::cout << "Original hit pos = " << hit.x << ", " << hit.y << std::endl;
        std::cout << "Rotated hit pos1 = " << hit_centred.first << ", " << hit_centred.second << std::endl;
        std::cout << "Rotated hit pos2 = " << hit_centred.first<< ", " << hit_centred.second << std::endl;
        std::cout << "Is in both rects? "  << inside_top_rect << ", " << inside_bottom_rect << std::endl;
        std::cout << std::endl << "---------------------" << std::endl;
    }

    return inside_top_rect && inside_bottom_rect;
}

static bool distanceStripComparator(const std::pair<double, Vector2D>& a, const std::pair<double, Vector2D>& b) {
    return a.first < b.first;
}

std::set<std::pair<Vector2D,Vector2D>> SCTModule::getHitStripsOneLayer(const std::vector<Vector2D>& strip_edges,  const std::vector<Hit>& hits) const
{   
    // Object to store results in 
    std::set<std::pair<Vector2D,Vector2D>> hit_strips{};

    // Loop through hits
    for (const auto& hit : hits)
    {
        // Check that hit actulaly lands on the module
        if (hit.layer != m_layer) continue;
        if (hit.charge == 0.) continue;
        if (!checkHitModuleOverlap(hit)){
            // std::cout << hit << " not on module -- skipping" << std::endl;
            continue;
        }

        std::vector<std::pair<double, Vector2D>> distances{};

        // For each strip edge, find the distance between hit and the edge
        for (const auto& strip_edge : strip_edges) 
        {
            double strip_to_hit_distance = strip_edge.distance_to_point({hit.x, hit.y}, m_length);
            distances.push_back({strip_to_hit_distance, strip_edge});
        }
        
        std::sort(distances.begin(), distances.end(), distanceStripComparator);  
        hit_strips.insert({distances[0].second, distances[1].second});
    }
    return hit_strips;
}

std::pair<std::set<std::pair<Vector2D,Vector2D>>, std::set<std::pair<Vector2D,Vector2D>>> SCTModule::getHitStrips(const std::vector<Hit>& hits) const
{   
    // Parameterise the hit strips in terms of pairs of strip edges
    std::set<std::pair<Vector2D,Vector2D>> hit_top_strips{};
    std::set<std::pair<Vector2D,Vector2D>> hit_bottom_strips{};

    hit_top_strips = getHitStripsOneLayer(m_top_strip_edges, hits);
    hit_bottom_strips = getHitStripsOneLayer(m_bottom_strip_edges, hits);
    
    return {hit_top_strips, hit_bottom_strips};
}


std::tuple<std::vector<Hit>, std::set<std::pair<Vector2D, Vector2D>>, std::set<std::pair<Vector2D, Vector2D>>> SCTModule::getSpacePoints(const std::vector<Hit>& hits) const
{
    std::pair<std::set<std::pair<Vector2D,Vector2D>>, std::set<std::pair<Vector2D,Vector2D>>> hit_strips = getHitStrips(hits);

    std::vector<Hit> space_points{};

    // Loop though each pair of top and bottom hit strip edges
    for (const auto& top_strip_edge_pairs: hit_strips.first){
    for (const auto& bottom_strip_edge_pairs: hit_strips.second){
        bool is_valid = true;
        

        // Work out where all strips intercept
        Vec2 intercept_11 = top_strip_edge_pairs.first.compute_intercept(bottom_strip_edge_pairs.first);
        Vec2 intercept_12 = top_strip_edge_pairs.first.compute_intercept(bottom_strip_edge_pairs.second);
        Vec2 intercept_21 = top_strip_edge_pairs.second.compute_intercept(bottom_strip_edge_pairs.first);
        Vec2 intercept_22 = top_strip_edge_pairs.second.compute_intercept(bottom_strip_edge_pairs.second);

        std::vector<Vec2> intercepts{intercept_11, intercept_12, intercept_21, intercept_22};

        // Check that all intercept points lie on the module
        for (const auto& intercept : intercepts)
        {   
            if (!checkHitModuleOverlap(Hit{intercept.x, intercept.y}))
            {
                is_valid = false;
            }
        }

        // If all intercepts are on module then construct a space-point
        if (is_valid)
        {
            Hit sp;
            sp.x = (intercept_11.x + intercept_12.x + intercept_21.x + intercept_22.x) / 4;
            sp.y = (intercept_11.y + intercept_12.y + intercept_21.y + intercept_22.y) / 4;
            sp.z = m_zpos;
            sp.layer  = m_layer;
            sp.charge = 1;

            // If space point is not already in the vector then push it back
            auto it = std::find(space_points.begin(), space_points.end(), sp);
            if (it == space_points.end()) 
            {   
                space_points.push_back(sp);
            }   
            
        }
    }
    }

    return {space_points, hit_strips.first, hit_strips.second};
}

void SCTModule::matchSpacePointsToHits(const std::vector<Hit>& hits, std::vector<Hit>& space_points) const
{
    for (const auto& hit: hits)
    {
        double smallest_distance{10e10};
        
        if (hit.layer != m_layer) continue;
        if (hit.charge == 0) continue;
        if (!checkHitModuleOverlap(hit)) continue;

        int best_idx{-1};
        int sp_index{0};

        for(const auto& sp : space_points)
        {   
            double distance = std::hypot(hit.x - sp.x, hit.y - sp.y);
            if (distance < smallest_distance)
            {   
                smallest_distance = distance;
                best_idx = sp_index;
            }
            sp_index++;
        }
        
        if (best_idx != -1)
        {
            space_points.at(best_idx).pdgc = hit.pdgc;
            space_points.at(best_idx).charge = hit.charge;
            space_points.at(best_idx).energy = hit.energy;
            space_points.at(best_idx).z = hit.z;
            space_points.at(best_idx).layer = hit.layer;
            space_points.at(best_idx).counter += 1;
        }
    }
}

std::vector<Hit> SCTModule::digitizeHits(const std::vector<Hit>& hits) const
{
    auto [space_points, top_hit_strip_pairs, bottom_hit_strip_pairs] = getSpacePoints(hits);
    matchSpacePointsToHits(hits, space_points);
    return space_points;
}



void SCTModule::drawHitsOnModule(const std::vector<Hit>& hits, int marker_size, int line_width) const
{
    TCanvas *c1 = new TCanvas("c1", "Hits", 800, 600);
    TGraph *true_hits_graph = new TGraph();
    TGraph *matched_hits_graph = new TGraph();
    TGraph *ghost_hits_graph = new TGraph();

    auto [space_points, top_hit_strip_pairs, bottom_hit_strip_pairs] = getSpacePoints(hits);

    std::cout << "Found " << space_points.size() << " space points" << std::endl;

    matchSpacePointsToHits(hits, space_points);


    int hit_counter{0};
    int ghost_hit_counter{0};
    int matched_hit_counter{0};
    for (const auto& sp: space_points)
    {   
        // Ghosts are defined as having pdgc == 0
        if (sp.pdgc == 0) 
        {   
            std::cout << "Ghost " << sp << std::endl;
            ghost_hits_graph->SetPoint(ghost_hit_counter, sp.x, sp.y);
            ghost_hit_counter++;
        }
        else
        {   
            std::cout << "Matched " << sp << std::endl;
            matched_hits_graph->SetPoint(matched_hit_counter, sp.x, sp.y);
            matched_hit_counter++;
        }
        
    }

    for (const auto& hit: hits)
    {   
        if (hit.layer != m_layer) continue;
        if (hit.charge == 0.) continue;
        if (!checkHitModuleOverlap(hit)) continue;
        std::cout << "True " << hit << std::endl;
        true_hits_graph->SetPoint(hit_counter, hit.x, hit.y);
        hit_counter++;
    }

    
    true_hits_graph->SetMarkerStyle(20);
    true_hits_graph->SetMarkerSize(marker_size);
    true_hits_graph->SetMarkerColor(kBlack);
    true_hits_graph->SetTitle("True Hits");
    
    matched_hits_graph->SetMarkerStyle(21);
    matched_hits_graph->SetMarkerSize(marker_size);
    matched_hits_graph->SetMarkerColor(kGreen);
    matched_hits_graph->SetTitle("Reco Hits");

    ghost_hits_graph->SetMarkerStyle(22);
    ghost_hits_graph->SetMarkerSize(marker_size);
    ghost_hits_graph->SetMarkerColor(kRed);
    ghost_hits_graph->SetTitle("Ghost Hits");
    
    true_hits_graph->Draw("AP");
    c1->Update();
    matched_hits_graph->Draw("P SAME");
    c1->Update();
    ghost_hits_graph->Draw("P SAME");
    c1->Update();

    bool firstDraw{true};
    for (const auto& strip_pair : top_hit_strip_pairs) {

        auto end1 = strip_pair.first.get_end(m_length);
        // TLine *line1 = new TLine(strip_pair.first.anchor.x, strip_pair.first.anchor.y, end1.x, end1.y);
        TGraph *line1 = new TGraph();
        line1->SetPoint(0, strip_pair.first.anchor.x, strip_pair.first.anchor.y);
        line1->SetPoint(1, end1.x, end1.y);
        line1->SetLineColor(kOrange);
        line1->SetLineWidth(line_width);
        line1->GetYaxis()->SetLimits(-1.5*m_length/2, 1.5*m_length/2);
        line1->GetXaxis()->SetLimits(-1.5*m_length/2, 1.5*m_length/2);

        // std::cout << "Anchor = " << strip_pair.first.anchor.x << ", " << strip_pair.first.anchor.y << std::endl;
        // std::cout << "End = " << end1.x << ", " << end1.y << std::endl;

        if (firstDraw)
        {
            line1->Draw("L SAME");
        }
        else
        {
            line1->Draw("L SAME");
        }
        
        c1->Update();

        auto end2 = strip_pair.second.get_end(m_length);
        TGraph *line2 = new TGraph();
        line2->SetPoint(0, strip_pair.second.anchor.x, strip_pair.second.anchor.y);
        line2->SetPoint(1, end2.x, end2.y);
        line2->SetLineColor(kOrange);
        line2->SetLineWidth(line_width);
        line2->GetYaxis()->SetLimits(-1.5*m_length/2, 1.5*m_length/2);
        line2->GetXaxis()->SetLimits(-1.5*m_length/2, 1.5*m_length/2);
        line2->Draw("L SAME");
        c1->Update();
    }

    for (const auto& strip_pair : bottom_hit_strip_pairs) {

        auto end1 = strip_pair.first.get_end(m_length);
        // TLine *line1 = new TLine(strip_pair.first.anchor.x, strip_pair.first.anchor.y, end1.x, end1.y);
        TGraph *line1 = new TGraph();
        line1->SetPoint(0, strip_pair.first.anchor.x, strip_pair.first.anchor.y);
        line1->SetPoint(1, end1.x, end1.y);
        line1->SetLineColor(kGray);
        line1->SetLineWidth(line_width);
        line1->GetYaxis()->SetLimits(-1.5*m_length/2, 1.5*m_length/2);
        line1->GetXaxis()->SetLimits(-1.5*m_length/2, 1.5*m_length/2);   
        line1->Draw("L SAME");
        // c1->Update();

        auto end2 = strip_pair.second.get_end(m_length);
        TGraph *line2 = new TGraph();
        line2->SetPoint(0, strip_pair.second.anchor.x, strip_pair.second.anchor.y);
        line2->SetPoint(1, end2.x, end2.y);
        line2->SetLineColor(kGray);
        line2->SetLineWidth(line_width);
        line2->GetYaxis()->SetLimits(-1.5*m_length/2, 1.5*m_length/2);
        line2->GetXaxis()->SetLimits(-1.5*m_length/2, 1.5*m_length/2);
        line2->Draw("L SAME");
        // c1->Update();

        // std::cout << "Anchor = " << strip_pair.first.anchor.x << ", " << strip_pair.first.anchor.y << std::endl;
        // std::cout << "End = " << end1.x << ", " << end1.y << std::endl;
        // std::cout << "------------------------------------------" << std::endl;
        // std::cout << "Anchor = " << strip_pair.second.anchor.x << ", " << strip_pair.second.anchor.y << std::endl;
        // std::cout << "End = " << end2.x << ", " << end2.y << std::endl;
        // std::cout << "------------------------------------------" << std::endl;

    }

    // true_hits_graph->Draw("P SAME");
    // true_hits_graph->Draw("AP SAME");
    // c1->Update();
    // matched_hits_graph->Draw("P SAME");
    // c1->Update();
    // ghost_hits_graph->Draw("P SAME");
    // c1->Update();

    true_hits_graph->GetYaxis()->SetLimits(-1.5*m_length/2, 1.5*m_length/2);
    true_hits_graph->GetXaxis()->SetLimits(-1.5*m_length/2, 1.5*m_length/2);
    
    matched_hits_graph->GetYaxis()->SetLimits(-1.5*m_length/2, 1.5*m_length/2);
    matched_hits_graph->GetXaxis()->SetLimits(-1.5*m_length/2, 1.5*m_length/2);

    ghost_hits_graph->GetYaxis()->SetLimits(-1.5*m_length/2, 1.5*m_length/2);
    ghost_hits_graph->GetXaxis()->SetLimits(-1.5*m_length/2, 1.5*m_length/2);

    std::cout << "Width " << m_width <<  std::endl;
    std::cout << "Length " << m_length <<  std::endl;

    // Add legend
    TLegend *legend = new TLegend(0.1, 0.7, 0.3, 0.9);
    legend->AddEntry(true_hits_graph, "True Hits", "p");
    legend->AddEntry(matched_hits_graph, "Matched Hits", "p");
    legend->AddEntry(ghost_hits_graph, "Ghost Hits", "p");
    legend->Draw()  ;
    c1->GetPad(0)->SetLeftMargin(0.30);
    // c1->GetPad(1)->SetBottomMargin(0.15);
    c1->Update();
    c1->SaveAs("hits.pdf");
}

std::ostream& operator<<(std::ostream& os, const SCTModule& module) {
    os << "SCTModule(position=[" << module.m_xpos << ", " << module.m_ypos << ", " << module.m_zpos << "]"
       << ", rotation=" << module.m_rotation
       << ", layer=" << module.m_layer
       << ")";
    return os;
}