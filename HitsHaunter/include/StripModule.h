#ifndef STRIPMODULE_H
#define STRIPMODULE_H

#include <iostream>
#include <vector>
#include <cmath>
#include <set>
#include <stdexcept>
#include <algorithm>
#include <array>
#include <utility>
#include <numeric>
#include <string>
#include <tuple>

#include <TCanvas.h>
#include <TGraph.h>
#include <TAxis.h>
#include <TLegend.h>
#include <TLine.h>

#include "Vector2D.h"
#include "Hit.h"

class SCTModule {
    
private:
    double m_xpos, m_ypos, m_zpos;
    int m_layer;
    double m_width, m_length;
    int m_nstrips;
    double m_stereo_angle;
    double m_rotation;
    std::string m_name;

    std::vector<Vector2D> m_top_strip_edges;
    std::vector<Vector2D> m_bottom_strip_edges;

    void initStripEdges();

    std::set<std::pair<Vector2D,Vector2D>> getHitStripsOneLayer(const std::vector<Vector2D>& strip_edges,  const std::vector<Hit>& hits) const;

public:
    SCTModule(double xpos, double ypos, double zpos, int layer, double rotation=0, bool flip=false, std::string name="", double width=63.56, double length=128.05, int nstrips=768, double stereo_angle=0.04);
    
    bool checkHitModuleOverlap(const Hit& hit, bool debug=false) const;

    std::pair<std::set<std::pair<Vector2D,Vector2D>>, std::set<std::pair<Vector2D,Vector2D>>> getHitStrips(const std::vector<Hit>& hits) const;

    std::tuple<std::vector<Hit>, std::set<std::pair<Vector2D, Vector2D>>, std::set<std::pair<Vector2D, Vector2D>>> getSpacePoints(const std::vector<Hit>& hits) const;

    void matchSpacePointsToHits(const std::vector<Hit>& hits, std::vector<Hit>& space_points) const;

    void drawHitsOnModule(const std::vector<Hit>& hits, int marker_size = 1, int line_width = 1, std::string plot_name = "Hits.pdf") const;
    
    std::vector<Hit> digitizeHits(const std::vector<Hit>& hits) const;

    friend std::ostream& operator<<(std::ostream& os, const SCTModule& module);

    int getLayerNum() const {return m_layer;}

};

#endif