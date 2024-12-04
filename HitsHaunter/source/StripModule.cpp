#include "StripModule.h"


StripModule::StripModule(double width, double length, int nstrips, double stereo_rotation, double x_pos, double y_pos) :
m_xlength{width}, m_ylength{length}, m_nstrips{nstrips}, m_stereo_rotation{stereo_rotation}, m_xpos{x_pos}, m_ypos{y_pos}
{}


StripModule::~StripModule(){}


StripModule::StripModule(const StripModule& other) :
m_xlength{other.m_xlength}, m_ylength{other.m_ylength}, m_nstrips{other.m_nstrips}, m_stereo_rotation{other.m_stereo_rotation}, m_xpos{other.m_xpos}, m_ypos{other.m_ypos}, m_front_lines{other.m_front_lines}
{}


StripModule& StripModule::operator=(const StripModule& other)
{   
    if (this == &other) { return *this; }

    m_xlength = other.m_xlength;
    m_ylength = other.m_ylength;
    m_nstrips = other.m_nstrips;
    m_stereo_rotation = other.m_stereo_rotation;
    m_xpos = other.m_xpos;
    m_ypos = other.m_ypos;
    
    m_front_lines = other.m_back_lines;

    return *this;
}

inline double StripModule::getStripWidth() const
{
    if (m_xlength > m_ylength) return m_xlength / m_nstrips;
    
    return m_ylength / m_nstrips;
}

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

void StripModule::initFrontStrips()
{
    // Form is x cos(theta) + y sin(theta) - p = 0
    // Ax + By = C
    double A{0.};
    double B{0.};
    double C{0.};  // strips below/left of centre
    double P{0.};

    int x_sign = sgn(m_xpos);
    int y_sign = sgn(m_ypos);
    double theta{0.};
    if (m_ylength > m_xlength)
    {
        theta = M_PI / 2 ;
    }

    if (m_xlength > m_ylength)
    {
        A = 0;  // Strips run along the x-direction
        B = 1 * y_sign;
        C = m_ypos; // Centre of strip
    }
    else
    {
        A = 1 * x_sign;  
        B = 0; // Strips run along the y-direction
        C = m_xpos; // Centre of strip
    }
    
    for (unsigned int i{0}; i < m_nstrips / 2; i++) // TODO: This evaluates to a whole number for ATLAS SCT - but should handle case where nstrips is an odd number
    {   
        C += getStripWidth(); // Move along one strip's width
        P = -C * pow(A*A + B*B, -0.5);
        m_front_lines.emplace_back(A, B, C);
        m_front_lines.emplace_back(A, B, -C);
    }

}


std::pair<Eigen::Vector3d, Eigen::Vector3d> StripModule::getStripLines(const Eigen::Vector3d& hit) const
{   
    double current_edge_pos{0};
    double next_edge_pos{getStripWidth()};
    for (unsigned int i{0}; i < m_nstrips; i++)
    {   
        if (hit.y() < next_edge_pos && hit.y() > current_edge_pos)
        {

        }
    }
}


std::pair<std::vector<Eigen::Vector3d>, std::vector<Eigen::Vector3d>> StripModule::getIntersectingStrips(const std::vector<Eigen::Vector3d>& hits, std::pair<std::vector<Eigen::Vector3d>, std::vector<Eigen::Vector3d>>& results) const
{
    for (const auto& hit : hits)
    {

    }
}