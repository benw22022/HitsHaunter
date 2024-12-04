#ifndef ROOTREADER_H
#define ROOTREADER_H

#include <utility>
#include <vector>
#include <Eigen/Dense>

class StripModule
{
    public:
        //TODO: Need to define (xpos, ypos) to be the centre of the SCT module for back-plane rotation 
        StripModule(double x_length, double y_length, int nstrips, double stereo_rotation, double x_pos, double y_pos);
        StripModule(const StripModule& other);
        StripModule& operator=(const StripModule& other);
        ~StripModule();

        void setWidth();
        void setLength();
        void setNStrips();
        void setStereoRotation();

        double getWidth() const;
        double getLength() const;
        int getNStrips() const;
        double getStereoRotation() const;
        inline double getStripWidth() const;

        std::pair<Eigen::Vector3d, Eigen::Vector3d> getStripLines(const Eigen::Vector3d& hit) const;
        std::pair<std::vector<Eigen::Vector3d>, std::vector<Eigen::Vector3d>> getIntersectingStrips(const std::vector<Eigen::Vector3d>& hits, std::pair<std::vector<Eigen::Vector3d>, std::vector<Eigen::Vector3d>>& results) const;
        

    private:

        double m_xlength;
        double m_ylength;
        int m_nstrips;
        double m_stereo_rotation;
        double m_xpos;
        double m_ypos;
        
        void initFrontStrips();
        void initBackStrips();

        std::vector<Eigen::Vector3d> m_front_lines;
        std::vector<Eigen::Vector3d> m_back_lines;

        

    
};

#endif