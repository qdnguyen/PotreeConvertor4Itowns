#ifndef PROJECTOR_HPP
#define PROJECTOR_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <proj_api.h>

#include "Vector3.h"

namespace Potree {


//std::string convert_lamb93_to_wgs84_in_double_precision(const double lmbx, const double lmby);
//void convert_wgs84_to_lamb93(double& lon_lmbx, double& lat_lmby, double& lmbz_eva);
//void convert_lamb93_to_wgs84(double& lmbx_lon, double& lmby_lat, double& lmbz_eva);
    
class Projector {
public:
    const double convert_to_degree = 180.0/3.141592653589793;
    const double convert_to_radian=  3.141592653589793/180.0;
    const double radiiX = 6378137.0;
    const double radiiY = 6378137.0;
    const double radiiZ = 6356752.3142451793;
    
    Projector() {
        // Initialize the Lambert93 projection
        
        if(!(Lambert93 = pj_init_plus("+init=IGNF:LAMB93"))) {
            std::cerr << "ERROR: [coordinate_convertions]: "
                      << "Lamber93 projection is not initialized" << std::endl;
        }

        // Initialize the WGS84G projection
        if(!(WGS84G = pj_init_plus("+init=IGNF:WGS84G"))) {
            std::cerr << "ERROR: [coordinate_convertions]: "
                      << "WGS84G projection is not initialized" << std::endl;
        }
        
        /*
        
        if(!(Lambert93 = pj_init_plus("+proj=lcc +nadgrids=ntf_r93.gsb,null +towgs84=-168.0000,-60.0000,320.0000 +a=6378249.2000 +rf=293.4660210000000 +pm=2.337229167 +lat_0=44.100000000 +lon_0=0.000000000 +k_0=0.99987750 +lat_1=44.100000000 +x_0=600000.000 +y_0=3200000.000 +units=m +no_defs"))) {
            std::cerr << "ERROR: [coordinate_convertions]: "
            << "Lamber93 projection is not initialized" << std::endl;
        }
        
        // Initialize the WGS84G projection
        if(!(WGS84G = pj_init_plus("+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs"))) {
            std::cerr << "ERROR: [coordinate_convertions]: "
            << "WGS84G projection is not initialized" << std::endl;
        }
         */
    
    }

    void convert_wgs84_to_lamb93x(double& lon_lmbx, double& lat_lmby, double& lmbz_eva);

    void convert_lamb93_to_wgs84x(double& lmbx_lon, double& lmby_lat, double& lmbz_eva);
    
    
    //Referentiel Cesium
    Vector3<double> carthographicToCartesian(Vector3<double> &carthographic);
    
    Vector3<double> geodeticSurfaceNormalCarthographic(double& longitude, double& latitude);
    
    //Referentiel IGN
    Vector3<double> carthographicToCartesianIGN(Vector3<double> &carthographic);
    
    Vector3<double> geodeticSurfaceNormalCarthographicIGN(double& longitude, double& latitude);
    

private:
    projPJ Lambert93;
    projPJ WGS84G;
};

}

#endif // PROJECTOR_HPP
