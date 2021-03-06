

#ifndef PLYPOINTREADER_H
#define PLYPOINTREADER_H

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include <boost/regex.hpp>
#include "boost/assign.hpp"
#include "boost/algorithm/string.hpp"

#include "Point.h"
#include "PointReader.h"

#include "Projector.hpp"

using std::ifstream;
using std::string;
using std::vector;
using std::map;
using std::cout;
using std::endl;
using namespace boost::assign;
using boost::split;
using boost::is_any_of;

namespace Potree{

const int PLY_FILE_FORMAT_ASCII = 0;
const int PLY_FILE_FORMAT_BINARY_LITTLE_ENDIAN = 1;

struct PlyPropertyType{
	string name;
	int size;

	PlyPropertyType(){}

	PlyPropertyType(string name, int size)
	:name(name)
	,size(size)
	{
	
	}
};

struct PlyProperty{
	string name;
	PlyPropertyType type;

	PlyProperty(string name, PlyPropertyType type)
		:name(name)
		,type(type)
	{

	}
};

struct PlyElement{
	string name;
	vector<PlyProperty> properties;
	int size;

	PlyElement(string name)
		:name(name)
	{

	}
};


map<string, PlyPropertyType> plyPropertyTypes = map_list_of
	("char", PlyPropertyType("char", 1))
	("int8", PlyPropertyType("char", 1))
	("uchar", PlyPropertyType("uchar", 1))
	("uint8", PlyPropertyType("uchar", 1))
	("short", PlyPropertyType("short", 2))
	("int16", PlyPropertyType("short", 2))
	("ushort", PlyPropertyType("ushort", 2))
	("uint16", PlyPropertyType("ushort", 2))
	("int", PlyPropertyType("int", 4))
	("int32", PlyPropertyType("int", 4))
	("uint", PlyPropertyType("uint", 4))
	("uint32", PlyPropertyType("uint", 4))
	("float", PlyPropertyType("float", 4))
	("float32", PlyPropertyType("float", 4))
	("double", PlyPropertyType("double", 8))
	("float64", PlyPropertyType("double", 8));

vector<string> plyRedNames = list_of("r")("red")("diffuse_red");
vector<string> plyGreenNames = list_of("g")("green")("diffuse_green");
vector<string> plyBlueNames = list_of("b")("blue")("diffuse_blue");

class PlyPointReader : public PointReader{
private:
	AABB *aabb;
	ifstream stream;
	int format;
	long pointCount;
	long pointsRead;
	PlyElement vertexElement;
	char *buffer;
	int pointByteSize;
	Point point;
	string file;
    double pivotX,pivotY,pivotZ;
    
public:
	PlyPointReader(string file)
	: stream(file, std::ios::in | std::ios::binary)
	,vertexElement("vertexElement"){
		format = -1;
		pointCount = 0;
		pointsRead = 0;
		pointByteSize = 0;
		buffer = new char[100];
		aabb = NULL;
		this->file = file;
        pivotX = pivotY = pivotZ = 0.0;

		boost::regex rEndHeader("^end_header.*");
		boost::regex rFormat("^format (ascii|binary_little_endian).*");
		boost::regex rElement("^element (\\w*) (\\d*)");
		boost::regex rProperty("^property (char|int8|uchar|uint8|short|int16|ushort|uint16|int|int32|uint|uint32|float|float32|double|float64) (\\w*)");
		
        boost::regex rPivot("^comment IGN offset Pos.*");
        //boost::regex rPivot("^comment IGN offset Pos (\\d{4}[- ]){3}\\d{4}");
        
		string line;
		while(std::getline(stream, line)){
			boost::trim(line);

			boost::cmatch sm;
			if(boost::regex_match(line, rEndHeader)){
				// stop line parsing when end_header is encountered
				break;
			}else if(boost::regex_match(line.c_str(), sm, rFormat)){
				// parse format
				string f = sm[1];
				if(f == "ascii"){
					format = PLY_FILE_FORMAT_ASCII;
				}else if(f == "binary_little_endian"){
					format = PLY_FILE_FORMAT_BINARY_LITTLE_ENDIAN;
				}
			}else if(boost::regex_match(line.c_str(), sm, rPivot)){
                std::istringstream iss(line);
                string word = "";
                iss >> word;
                if(word == "comment"){
                    iss >> word;
                    if(word == "IGN"){
                        iss >> word;
                        if(word == "offset" || word == "Offset"){
                            iss >> word;
                            if(word == "Pos"){
                                iss >> pivotX >> pivotY >> pivotZ;
                            }
                        }
                    }
                }

            
            }else if(boost::regex_match(line.c_str(), sm, rElement)){
				// parse vertex element declaration
				string name = sm[1];
				long count = atol(string(sm[2]).c_str());

				if(name != "vertex"){
					continue;
				}
				pointCount = count;

				while(true){
					std::streamoff len = stream.tellg();
					getline(stream, line);
					boost::trim(line);
					if(boost::regex_match(line.c_str(), sm, rProperty)){
						string name = sm[2];
						PlyPropertyType type = plyPropertyTypes[sm[1]];
						PlyProperty property(name, type);
						vertexElement.properties.push_back(property);
						pointByteSize += type.size;
					}else{
						// abort if line was not a property definition
						stream.seekg(len ,std::ios_base::beg);
						break;
					}
				}
			}
		}
	}

    bool readNextPoint(){
        if(pointsRead == pointCount){
            return false;
        }
        
        double x = 0;
        double y = 0;
        double z = 0;
        float dummy;
        float nx = 0;
        float ny = 0;
        float nz = 0;
        unsigned char r = 0;
        unsigned char g = 0;
        unsigned char b = 0;
        
        if(format == PLY_FILE_FORMAT_ASCII){
            string line;
            getline(stream, line);
            boost::trim(line);
            
            vector<string> tokens;
            split(tokens, line, is_any_of("\t "));
            int i = 0;
            for(const auto &prop : vertexElement.properties){
                string token = tokens[i++];
                if(prop.name == "x" && prop.type.name == plyPropertyTypes["float"].name){
                    x = stof(token);
                }else if(prop.name == "y" && prop.type.name == plyPropertyTypes["float"].name){
                    y = stof(token);
                }else if(prop.name == "z" && prop.type.name == plyPropertyTypes["float"].name){
                    z = stof(token);
                }else if(prop.name == "x" && prop.type.name == plyPropertyTypes["double"].name){
                    x = stod(token);
                }else if(prop.name == "y" && prop.type.name == plyPropertyTypes["double"].name){
                    y = stod(token);
                }else if(prop.name == "z" && prop.type.name == plyPropertyTypes["double"].name){
                    z = stod(token);
                }else if(std::find(plyRedNames.begin(), plyRedNames.end(), prop.name) != plyRedNames.end() && prop.type.name == plyPropertyTypes["uchar"].name){
                    r = (unsigned char)stof(token);
                }else if(std::find(plyGreenNames.begin(), plyGreenNames.end(), prop.name) != plyGreenNames.end() && prop.type.name == plyPropertyTypes["uchar"].name){
                    g = (unsigned char)stof(token);
                }else if(std::find(plyBlueNames.begin(), plyBlueNames.end(), prop.name) != plyBlueNames.end() && prop.type.name == plyPropertyTypes["uchar"].name){
                    b = (unsigned char)stof(token);
                }else if(prop.name == "nx" && prop.type.name == plyPropertyTypes["float"].name){
                    nx = stof(token);
                }else if(prop.name == "ny" && prop.type.name == plyPropertyTypes["float"].name){
                    ny = stof(token);
                }else if(prop.name == "nz" && prop.type.name == plyPropertyTypes["float"].name){
                    nz = stof(token);
                }
            }
        }else if(format == PLY_FILE_FORMAT_BINARY_LITTLE_ENDIAN){
            stream.read(buffer, pointByteSize);
            
            int offset = 0;
            for(const auto &prop : vertexElement.properties){
                if(prop.name == "x" && prop.type.name == plyPropertyTypes["float"].name){
                    memcpy(&dummy, (buffer+offset), prop.type.size);
                    x=dummy;
                }else if(prop.name == "y" && prop.type.name == plyPropertyTypes["float"].name){
                    memcpy(&dummy, (buffer+offset), prop.type.size);
                    y=dummy;
                }else if(prop.name == "z" && prop.type.name == plyPropertyTypes["float"].name){
                    memcpy(&dummy, (buffer+offset), prop.type.size);
                    z=dummy;
                }else if(prop.name == "x" && prop.type.name == plyPropertyTypes["double"].name){
                    memcpy(&x, (buffer+offset), prop.type.size);
                }else if(prop.name == "y" && prop.type.name == plyPropertyTypes["double"].name){
                    memcpy(&y, (buffer+offset), prop.type.size);
                }else if(prop.name == "z" && prop.type.name == plyPropertyTypes["double"].name){
                    memcpy(&z, (buffer+offset), prop.type.size);
                }else if(std::find(plyRedNames.begin(), plyRedNames.end(), prop.name) != plyRedNames.end() && prop.type.name == plyPropertyTypes["uchar"].name){
                    memcpy(&r, (buffer+offset), prop.type.size);
                }else if(std::find(plyGreenNames.begin(), plyGreenNames.end(), prop.name) != plyGreenNames.end() && prop.type.name == plyPropertyTypes["uchar"].name){
                    memcpy(&g, (buffer+offset), prop.type.size);
                }else if(std::find(plyBlueNames.begin(), plyBlueNames.end(), prop.name) != plyBlueNames.end() && prop.type.name == plyPropertyTypes["uchar"].name){
                    memcpy(&b, (buffer+offset), prop.type.size);
                }else if(prop.name == "nx" && prop.type.name == plyPropertyTypes["float"].name){
                    memcpy(&nx, (buffer+offset), prop.type.size);
                }else if(prop.name == "ny" && prop.type.name == plyPropertyTypes["float"].name){
                    memcpy(&ny, (buffer+offset), prop.type.size);
                }else if(prop.name == "nz" && prop.type.name == plyPropertyTypes["float"].name){
                    memcpy(&nz, (buffer+offset), prop.type.size);
                }
                
                
                offset += prop.type.size;
            }
            
        }
        
        
        
        point = Point(x,y,z,r,g,b);
        
        point.normal.x = nx;
        point.normal.y = ny;
        point.normal.z = nz;
        pointsRead++;
        return true;
    }
    
	bool readNextPoint(string projection){
		if(pointsRead == pointCount){
			return false;
		}
		
		double x = 0;
		double y = 0;
		double z = 0;
		float dummy;
		float nx = 0;
		float ny = 0;
		float nz = 0;
		unsigned char r = 0;
		unsigned char g = 0;
		unsigned char b = 0;

		if(format == PLY_FILE_FORMAT_ASCII){
			string line;
			getline(stream, line);
			boost::trim(line);

			vector<string> tokens;
			split(tokens, line, is_any_of("\t "));
			int i = 0;
			for(const auto &prop : vertexElement.properties){
				string token = tokens[i++];
				if(prop.name == "x" && prop.type.name == plyPropertyTypes["float"].name){
					x = stof(token);
				}else if(prop.name == "y" && prop.type.name == plyPropertyTypes["float"].name){
					y = stof(token);
				}else if(prop.name == "z" && prop.type.name == plyPropertyTypes["float"].name){
					z = stof(token);
				}else if(prop.name == "x" && prop.type.name == plyPropertyTypes["double"].name){
					x = stod(token);
				}else if(prop.name == "y" && prop.type.name == plyPropertyTypes["double"].name){
					y = stod(token);
				}else if(prop.name == "z" && prop.type.name == plyPropertyTypes["double"].name){
					z = stod(token);
				}else if(std::find(plyRedNames.begin(), plyRedNames.end(), prop.name) != plyRedNames.end() && prop.type.name == plyPropertyTypes["uchar"].name){
					r = (unsigned char)stof(token);
				}else if(std::find(plyGreenNames.begin(), plyGreenNames.end(), prop.name) != plyGreenNames.end() && prop.type.name == plyPropertyTypes["uchar"].name){
					g = (unsigned char)stof(token);
				}else if(std::find(plyBlueNames.begin(), plyBlueNames.end(), prop.name) != plyBlueNames.end() && prop.type.name == plyPropertyTypes["uchar"].name){
					b = (unsigned char)stof(token);
				}else if(prop.name == "nx" && prop.type.name == plyPropertyTypes["float"].name){
					nx = stof(token);
				}else if(prop.name == "ny" && prop.type.name == plyPropertyTypes["float"].name){
					ny = stof(token);
				}else if(prop.name == "nz" && prop.type.name == plyPropertyTypes["float"].name){
					nz = stof(token);
				}
			}
		}else if(format == PLY_FILE_FORMAT_BINARY_LITTLE_ENDIAN){
			stream.read(buffer, pointByteSize);

			int offset = 0;
			for(const auto &prop : vertexElement.properties){
				if(prop.name == "x" && prop.type.name == plyPropertyTypes["float"].name){
					memcpy(&dummy, (buffer+offset), prop.type.size);
					x=dummy;
				}else if(prop.name == "y" && prop.type.name == plyPropertyTypes["float"].name){
					memcpy(&dummy, (buffer+offset), prop.type.size);
					y=dummy;
				}else if(prop.name == "z" && prop.type.name == plyPropertyTypes["float"].name){
					memcpy(&dummy, (buffer+offset), prop.type.size);
					z=dummy;
				}else if(prop.name == "x" && prop.type.name == plyPropertyTypes["double"].name){
					memcpy(&x, (buffer+offset), prop.type.size);
				}else if(prop.name == "y" && prop.type.name == plyPropertyTypes["double"].name){
					memcpy(&y, (buffer+offset), prop.type.size);
				}else if(prop.name == "z" && prop.type.name == plyPropertyTypes["double"].name){
					memcpy(&z, (buffer+offset), prop.type.size);
				}else if(std::find(plyRedNames.begin(), plyRedNames.end(), prop.name) != plyRedNames.end() && prop.type.name == plyPropertyTypes["uchar"].name){
					memcpy(&r, (buffer+offset), prop.type.size);
				}else if(std::find(plyGreenNames.begin(), plyGreenNames.end(), prop.name) != plyGreenNames.end() && prop.type.name == plyPropertyTypes["uchar"].name){
					memcpy(&g, (buffer+offset), prop.type.size);
				}else if(std::find(plyBlueNames.begin(), plyBlueNames.end(), prop.name) != plyBlueNames.end() && prop.type.name == plyPropertyTypes["uchar"].name){
					memcpy(&b, (buffer+offset), prop.type.size);
				}else if(prop.name == "nx" && prop.type.name == plyPropertyTypes["float"].name){
					memcpy(&nx, (buffer+offset), prop.type.size);
				}else if(prop.name == "ny" && prop.type.name == plyPropertyTypes["float"].name){
					memcpy(&ny, (buffer+offset), prop.type.size);
				}else if(prop.name == "nz" && prop.type.name == plyPropertyTypes["float"].name){
					memcpy(&nz, (buffer+offset), prop.type.size);
				}
				

				offset += prop.type.size;
			}
			
		}
        
        Vector3<double> position(x,y,z);
        
        //add pivot if found it in comment
        if(pivotX != 0.0 || pivotY != 0.0 || pivotZ != 0.0){
            position.x += pivotX;
            position.y += pivotY;
            position.z += pivotZ;
        }
        
        //Reprojection coordinates here
        if(projection == "LAMBERT2WGS84"){
            Projector projector;
            projector.convert_lamb93_to_wgs84x(position.x,position.y,position.z);
            projector.carthographicToCartesianIGN(position);
        }
		
        
        point = Point(position.x,position.y,position.z,r,g,b);
        
		point.normal.x = nx;
		point.normal.y = ny;
		point.normal.z = nz;
		pointsRead++;
		return true;
	}

	Point getPoint(){
		return point;
	}

    AABB getAABB(){
        if(aabb == NULL) aabb = new AABB();
        
        return *aabb;
            
    }
    
	AABB getAABB(string projection){
		if(aabb == NULL){

			aabb = new AABB();

			PlyPointReader *reader = new PlyPointReader(file);
            
			while(reader->readNextPoint(projection)){
				Point p = reader->getPoint();
                
				aabb->update(p.position);
			}

			reader->close();
			delete reader;

		}

		return *aabb;
	}

	long numPoints(){
		return pointCount;
	}

	void close(){
		stream.close();
	}


};

}

#endif
