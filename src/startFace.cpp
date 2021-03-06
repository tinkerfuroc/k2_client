#include "k2_client/k2_client.h"
#include "k2_client/FaceArray.h"
#include <iconv.h>
#include <cstdio>

std::string topicName = "faceArray";
size_t streamSize = 60000;
size_t readSkipSize = 60000;
size_t stringSize = 30000;

int main(int argC,char **argV)
{
    ros::init(argC,argV,"startFace");
    ros::NodeHandle n;
    std::string serverAddress;
    n.getParam("/serverNameOrIP",serverAddress);
    Socket mySocket(serverAddress.c_str(),"9006",streamSize);
    ros::Publisher facePub = n.advertise<k2_client::FaceArray>(topicName,1);
    while(ros::ok())
    {
        mySocket.readData();
        std::string jsonString(mySocket.mBuffer);
        Json::Value jsonObject;
        Json::Reader jsonReader;
        bool parsingSuccessful = jsonReader.parse(jsonString,jsonObject,false);
        if(!parsingSuccessful)
        {
            ROS_ERROR("Failure to parse");
            continue;
        }
        k2_client::FaceArray faceArray;
        try
        {
            for(int i=0;i<6;i++)
            {
                k2_client::Face face;
                face.header.stamp = ros::Time::now();
                face.header.frame_id =  ros::this_node::getNamespace().substr(1,std::string::npos) + "/depthFrame";
                
                face.appearance.wearingGlasses = jsonObject[i]["FaceProperties"]["WearingGlasses"].asBool();
                face.activities.eyeLeftClosed = jsonObject[i]["FaceProperties"]["EyeLeftClosed"].asBool();
                face.activities.eyeRightClosed = jsonObject[i]["FaceProperties"]["EyeRightClosed"].asBool();
                face.activities.mouthOpen = jsonObject[i]["FaceProperties"]["MouthOpen"].asBool();
                face.activities.mouthMoved = jsonObject[i]["FaceProperties"]["MouthMoved"].asBool();
                face.activities.lookingAway = jsonObject[i]["FaceProperties"]["LookingAway"].asBool();
                face.expressions.neutral = jsonObject[i]["FaceProperties"]["Neutral"].asBool();
                face.expressions.neutral = jsonObject[i]["FaceProperties"]["Happy"].asBool();
                
                face.facePointsInInfraredSpace.eyeLeftX = jsonObject[i]["FacePointsInInfraredSpace"]["EyeLeft"]["X"].asDouble();
                face.facePointsInInfraredSpace.eyeLeftY = jsonObject[i]["FacePointsInInfraredSpace"]["EyeLeft"]["Y"].asDouble();
                face.facePointsInInfraredSpace.eyeRightX = jsonObject[i]["FacePointsInInfraredSpace"]["EyeRight"]["X"].asDouble();
                face.facePointsInInfraredSpace.eyeRightY = jsonObject[i]["FacePointsInInfraredSpace"]["EyeRight"]["Y"].asDouble();
                face.facePointsInInfraredSpace.noseX = jsonObject[i]["FacePointsInInfraredSpace"]["EyeRight"]["X"].asDouble();
                face.facePointsInInfraredSpace.noseY = jsonObject[i]["FacePointsInInfraredSpace"]["EyeRight"]["Y"].asDouble();
                face.facePointsInInfraredSpace.mouthCornerLeftX = jsonObject[i]["FacePointsInInfraredSpace"]["MouthCornerLeft"]["X"].asDouble();
                face.facePointsInInfraredSpace.mouthCornerLeftY = jsonObject[i]["FacePointsInInfraredSpace"]["MouthCornerLeft"]["Y"].asDouble();
                face.facePointsInInfraredSpace.mouthCornerRightX = jsonObject[i]["FacePointsInInfraredSpace"]["MouthCornerRight"]["X"].asDouble();
                face.facePointsInInfraredSpace.mouthCornerRightY = jsonObject[i]["FacePointsInInfraredSpace"]["MouthCornerRight"]["Y"].asDouble();
                
                face.facePointsInColorSpace.eyeLeftX = jsonObject[i]["FacePointsInColorSpace"]["EyeLeft"]["X"].asDouble();
                face.facePointsInColorSpace.eyeLeftY = jsonObject[i]["FacePointsInColorSpace"]["EyeLeft"]["Y"].asDouble();
                face.facePointsInColorSpace.eyeRightX = jsonObject[i]["FacePointsInColorSpace"]["EyeRight"]["X"].asDouble();
                face.facePointsInColorSpace.eyeRightY = jsonObject[i]["FacePointsInColorSpace"]["EyeRight"]["Y"].asDouble();
                face.facePointsInColorSpace.noseX = jsonObject[i]["FacePointsInColorSpace"]["EyeRight"]["X"].asDouble();
                face.facePointsInColorSpace.noseY = jsonObject[i]["FacePointsInColorSpace"]["EyeRight"]["Y"].asDouble();
                face.facePointsInColorSpace.mouthCornerLeftX = jsonObject[i]["FacePointsInColorSpace"]["MouthCornerLeft"]["X"].asDouble();
                face.facePointsInColorSpace.mouthCornerLeftY = jsonObject[i]["FacePointsInColorSpace"]["MouthCornerLeft"]["Y"].asDouble();
                face.facePointsInColorSpace.mouthCornerRightX = jsonObject[i]["FacePointsInColorSpace"]["MouthCornerRight"]["X"].asDouble();
                face.facePointsInColorSpace.mouthCornerRightY = jsonObject[i]["FacePointsInColorSpace"]["MouthCornerRight"]["Y"].asDouble();
                
                face.faceBoundingBoxInInfraredSpace.left   = jsonObject[i]["FaceBoundingBoxInInfraredSpace"]["Left"].asInt();
                face.faceBoundingBoxInInfraredSpace.top    = jsonObject[i]["FaceBoundingBoxInInfraredSpace"]["Top"].asInt();
                face.faceBoundingBoxInInfraredSpace.right  = jsonObject[i]["FaceBoundingBoxInInfraredSpace"]["Right"].asInt();
                face.faceBoundingBoxInInfraredSpace.bottom = jsonObject[i]["FaceBoundingBoxInInfraredSpace"]["Bottom"].asInt();
                
                face.faceBoundingBoxInColorSpace.left   = jsonObject[i]["FaceBoundingBoxInColorSpace"]["Left"].asInt();
                face.faceBoundingBoxInColorSpace.top    = jsonObject[i]["FaceBoundingBoxInColorSpace"]["Top"].asInt();
                face.faceBoundingBoxInColorSpace.right  = jsonObject[i]["FaceBoundingBoxInColorSpace"]["Right"].asInt();
                face.faceBoundingBoxInColorSpace.bottom = jsonObject[i]["FaceBoundingBoxInColorSpace"]["Bottom"].asInt();
                
                face.faceRotationQuaternion.X = jsonObject[i]["FaceRotationQuaternion"]["X"].asDouble();
                face.faceRotationQuaternion.Y = jsonObject[i]["FaceRotationQuaternion"]["Y"].asDouble();
                face.faceRotationQuaternion.Z = jsonObject[i]["FaceRotationQuaternion"]["Z"].asDouble();
                face.faceRotationQuaternion.W = jsonObject[i]["FaceRotationQuaternion"]["W"].asDouble();
                
                //face.trackingId = jsonObject[i]["TrackingId"].asInt();
                face.faceFrameFeatures = jsonObject[i]["FaceFrameFeatures"].asInt();
                
                faceArray.faces.push_back(face);
            }
        }
        catch (...)
        {
            ROS_ERROR("An exception occured");
            continue;
        }
        if (faceArray.faces.size() > 0) 
            facePub.publish(faceArray);
    }
    return 0;
}
