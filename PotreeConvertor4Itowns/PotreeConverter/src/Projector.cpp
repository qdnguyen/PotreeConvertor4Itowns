#include "Projector.hpp"

namespace Potree {

void Projector::convert_lamb93_to_wgs84x(double& lmbx_lon, double& lmby_lat, double& lmbz_eva) {

    // transform
    pj_transform(Lambert93, WGS84G, 1, 0, &lmbx_lon, &lmby_lat, &lmbz_eva);

    //lmbx_lon *= convert_to_degree;
    //lmby_lat *= convert_to_degree;
    //lmbz_eva *= coordinate_convertions::convert_to_degree;
}


void Projector::convert_wgs84_to_lamb93x(double& lon_lmbx, double& lat_lmby, double& lmbz_eva) {
    
    // transform the coordinates
    lon_lmbx *= convert_to_radian;
    lat_lmby *= convert_to_radian;
    pj_transform(WGS84G, Lambert93, 1, 0, &lon_lmbx, &lat_lmby, &lmbz_eva);

}

    
Vector3<double> Projector::carthographicToCartesian(Vector3<double> &carthographic){
    
    double longitude = carthographic.x;
    double latitude  = carthographic.y;
    double height    = carthographic.z;
    
    Vector3<double> normal, K;
    
    normal = geodeticSurfaceNormalCarthographic(longitude, latitude);

    K = normal.multipleComponents(Vector3<double>(radiiX*radiiX,radiiY*radiiY,radiiZ*radiiZ));
    
    double gamma  = sqrt(K.dot(normal));
    
    K.divideByScalar(gamma);
    normal.multiplyByScalar(height);
    K.add(normal);
    carthographic.x = K.x;
    carthographic.y = K.y;
    carthographic.z = K.z;
    
    return K;
}

Vector3<double> Projector::geodeticSurfaceNormalCarthographic(double& longitude, double& latitude){

    double  cosLatitude = cos(latitude);
    
    double x = cosLatitude*cos(longitude);
    double y = cosLatitude*sin(longitude);
    double z = sin(latitude);
    
    Vector3<double> result(x,y,z);

    result.normalize();

    return result;
}
    
Vector3<double> Projector::geodeticSurfaceNormalCarthographicIGN(double& longitude, double& latitude){

    
    double  cosLatitude = cos(latitude);
    
    double x = cosLatitude*cos(-longitude);
    double y = cosLatitude*sin(-longitude);
    double z = sin(latitude);
    
    Vector3<double> result(x,z,y);
    
    result.normalize();
    
    return result;
        
}
    

Vector3<double> Projector::carthographicToCartesianIGN(Vector3<double> &carthographic){
        
        double longitude = carthographic.x + 3.141592653589793; //translate repere to 0,0
        double latitude  = carthographic.y;
        double height    = carthographic.z;
        
        Vector3<double> normal, K;
        
        normal = geodeticSurfaceNormalCarthographicIGN(longitude, latitude);
    
        //it should be squaire of X,Z,Y, not X,Y,Z???
        
        K = normal.multipleComponents(Vector3<double>(radiiX*radiiX,radiiY*radiiY,radiiZ*radiiZ));
        
        double gamma  = sqrt(K.dot(normal));
        
        K.divideByScalar(gamma);
        normal.multiplyByScalar(height);
        //K.add(normal);
        K.sub(normal);
        carthographic.x = K.x;
        carthographic.y = K.y;
        carthographic.z = K.z;
        
        return K;
}


}
