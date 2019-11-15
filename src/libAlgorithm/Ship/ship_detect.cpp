//
// Created by hbk on 19-4-19.
//

#include <stdio.h>  // for snprintf
#include <string>
#include <vector>
#include <math.h>
#include <fstream>
#include <boost/python.hpp>
//#include "caffe/caffe.hpp"
//#include "rotate_polygon_nms.hpp"
//#include "gpu_nms.hpp"
//#include "rotate_gpu_nms.hpp"
#include "rotate_polygon_nms.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "ship_detect.h"

using namespace caffe;

#define max(a, b) (((a)>(b)) ? (a) :(b))
#define min(a, b) (((a)<(b)) ? (a) :(b))

#define PI 3.1415926
const int class_num=5;   // �����?
const float reduce_scale = 1.4;  // C++ interface should be dived with predicted height and width  :  h /= reduce_scale;  w /= reduce_scale;

//Using for box sort
struct Info {
    float score;
    const float* head;
};
bool compare(const Info& Info1, const Info& Info2) {
    return Info1.score > Info2.score;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Detector
 *  Description:  Load the model file and weights file
 * =====================================================================================
 */
ShipDetector::ShipDetector(const string& model_file, const string& weights_file)
{
//    std::cout << "in sshipDetector and : \n";
//    std::cout << "model_file : " << model_file << "\n";
//    std::cout << "weights_file : " << weights_file << "\n";
    
    net_.reset(new Net<float>(model_file, TEST));
    net_->CopyTrainedLayersFrom(weights_file);

    std::cout<<"ship model is loading over ..." << std::endl;

}

/*
 * ===  FUNCTION  ======================================================================
 *            Name:  Detect
 *  return format : [ label score x1 y1 x2 y2 x3 y3 x4 y4]
 *     Description:  Perform detection operation
 *                    Warning the max input size should less than 1000*600
 * =====================================================================================
 */
std::vector<vector<float> > ShipDetector::Detection(const cv::Mat &cv_img)
{

    vector<vector<float> > detections;
    float CONF_THRESH = 0.8;
    float NMS_THRESH = 0.3;

    // ������ֵ������Ӧ��lib/fast_rcnn/config.py�е�TEST.SCALES��TEST.MAX_SIZE�еõ�
    const int  max_input_side=1700;   //  1700
    const int  min_input_side=1000;    // 1000
    
    if(cv_img.empty())
    {
        std::cout<<"Can not get the image file !"<<std::endl;
        return detections;
    }

    //cv::Mat cv_img = cv::imread(im_name);
    cv::Mat cv_new(cv_img.rows, cv_img.cols, CV_32FC3, cv::Scalar(0,0,0));
    
    int max_side = max(cv_img.rows, cv_img.cols);
    int min_side = min(cv_img.rows, cv_img.cols);

    int num_out;
    cv::Mat cv_resized;

    // ############## python ################
    // change the image's shorter side is 1000 (and the longer side isnot longer than max_input_side) or the longer side is 1700.
    float img_scale = (float)min_input_side / (float)min_side;
    if (img_scale * (float)max_side > (float)max_input_side)
    {
        img_scale = (float)max_input_side / (float)max_side;
    }
    int height = int(cv_img.rows * img_scale);
    int width = int(cv_img.cols * img_scale);
    //std::cout<<"resized height : "<<height << ",width : "<<width<<std::endl;
    // ######################################

    float im_info[3];
    float *data_buf = new float[height*width*3];
    float *boxes = nullptr;
    float *pred = nullptr;
    float *pred_per_class = nullptr;
    float *sorted_pred_cls = nullptr;
    int *keep = nullptr;
    const float* bbox_delt;
    const float* rois;
    const float* pred_cls;
    int num;

    for (int h = 0; h < cv_img.rows; ++h )
    {
        for (int w = 0; w < cv_img.cols; ++w)
        {
            cv_new.at<cv::Vec3f>(cv::Point(w, h))[0] = float(cv_img.at<cv::Vec3b>(cv::Point(w, h))[0])-float(102.9801);
            cv_new.at<cv::Vec3f>(cv::Point(w, h))[1] = float(cv_img.at<cv::Vec3b>(cv::Point(w, h))[1])-float(115.9465);
            cv_new.at<cv::Vec3f>(cv::Point(w, h))[2] = float(cv_img.at<cv::Vec3b>(cv::Point(w, h))[2])-float(122.7717);

        }
    }

    cv::resize(cv_new, cv_resized, cv::Size(width, height));
    im_info[0] = cv_resized.rows;
    im_info[1] = cv_resized.cols;
    im_info[2] = img_scale;

    for (int h = 0; h < height; ++h )
    {
        for (int w = 0; w < width; ++w)
        {
            data_buf[(0*height+h)*width+w] = float(cv_resized.at<cv::Vec3f>(cv::Point(w, h))[0]);
            data_buf[(1*height+h)*width+w] = float(cv_resized.at<cv::Vec3f>(cv::Point(w, h))[1]);
            data_buf[(2*height+h)*width+w] = float(cv_resized.at<cv::Vec3f>(cv::Point(w, h))[2]);
        }
    }

    net_->blob_by_name("data")->Reshape(1, 3, height, width);
    net_->blob_by_name("data")->set_cpu_data(data_buf);
    net_->blob_by_name("im_info")->set_cpu_data(im_info);
    net_->ForwardFrom(0);
    bbox_delt = net_->blob_by_name("bbox_pred")->cpu_data();  // box_deltas : ÿ5��һ�飬ÿclass_num��Ϊһ���߽�������ع�?    // python : shape (300,25) (num,5*class_num)  [ dx, dy, dw, dh, da]

    num = net_->blob_by_name("rois")->num();


    rois = net_->blob_by_name("rois")->cpu_data();            // rois ÿ6��һ�飬һ��ĺ�?����ʾbox��λ����Ϣ����һ����Ϣδ֪
    // python : shape (300,6)   [ ..., cx, cy, h, w, angle ]

    pred_cls = net_->blob_by_name("cls_prob")->cpu_data();    // ÿnum_classһ�飬һ��num�飬����һ���߽��������ĵ÷֣��ڶ����߽��������ĵ÷֣�...
    // python : shape (300,5)  ==> C++ : 5 + 5 + 5 + ... + 5

    boxes = new float[num*5];
    pred = new float[num*6*class_num];
    pred_per_class = new float[num*6];
    sorted_pred_cls = new float[num*6];
    keep = new int[num];

    // boxes : [cx, cy, h, w, angle]
    for (int n = 0; n < num; n++)
    {
        for (int c = 0; c < 5; c++)
        {
            // boxes��5��Ϊһ���߽������꣬��Ӧ��roisλ��Ϊ1,2,3,4,5��7,8,9,10,11��13,14,15,16,17��...4
            // rois : ÿ5��Ϊһ��
            boxes[n*5+c] = rois[n*6+c+1] / img_scale;
        }
        boxes[n*5+4] *= img_scale;   // �ǶȺ�ͼƬ�ĳ߶��޹أ�����Ҫ�ٻظ�ԭ���ĽǶȴ�С
    }

    // �õ� pred : [ctr_x,ctr_y,w,h,angle,conf_pred] : 6 * 5 * 300
    // ÿ6��(ctr_x,ctr_y,w,h,angle,conf_pred)һ�飬����ǰnum��Ϊclass[0]��Ӧ��num���߽����num��Ϊclass[1]��Ӧ��num���߽��?..
    bbox_transform_inv(num, bbox_delt, pred_cls, boxes, pred);

    for (int i = 1; i < class_num; i ++) {
        for (int j = 0; j < num; j++) {
            for (int k = 0; k < 6; k++)
                pred_per_class[j * 6 + k] = pred[(i * num + j) * 6 + k];  // class[0]Ϊ����background�����Խ�ǰnum��ɾȥ������(num+1)�鿪ʼ��ȡ���ݵ�pred_per_class��

            // hbk ==> ��֪��ΪʲôC++Ԥ��Ľ����͸߱�pythonԤ��Ĵ�?.4�������Կ�͸�Ҫ����?.4
            // hbk ==> I donot know why to div the scale( 1.4 )
            // hbk ==> but if not,the result is bigger than correct result
            pred_per_class[j * 6 + 2] /= reduce_scale;
            pred_per_class[j * 6 + 3] /= reduce_scale;

        }
        boxes_sort(num, pred_per_class, sorted_pred_cls);
        _rotate_nms(keep, &num_out, sorted_pred_cls, num, 6, NMS_THRESH, 0);
        //vis_detections(cv_img, keep, num_out, sorted_pred_cls, CONF_THRESH,i);

        float score, cx, cy, w, h, angle;
        std::vector<cv::Point> points;
        std::vector<float> detection(10);
        for (int m = 0; m < num_out; ++m) {
            score = sorted_pred_cls[keep[m] * 6 + 5];
            if (score > CONF_THRESH) {

                cx = sorted_pred_cls[keep[m] * 6 + 0];     // ctr_x
                cy = sorted_pred_cls[keep[m] * 6 + 1];     // ctr_y
                w = sorted_pred_cls[keep[m] * 6 + 2];      // w
                h = sorted_pred_cls[keep[m] * 6 + 3];      // h
                angle = sorted_pred_cls[keep[m] * 6 + 4];  // angle

                points = computeLocation(cx, cy, h, w, angle);  // convert box location (x,y,h,w,angle) to four points

                detection[0] = i;                // label
                detection[1] = score;            // score
                detection[2] = points[0].x;      // x1
                detection[3] = points[0].y;      // y1
                detection[4] = points[1].x;      // x2
                detection[5] = points[1].y;      // y2
                detection[6] = points[2].x;      // x3
                detection[7] = points[2].y;      // y3
                detection[8] = points[3].x;      // x4
                detection[9] = points[3].y;      // y4
                detections.push_back(detection);
            }
        }



    }
    //cv::imwrite("vis.jpg",cv_img);
    delete []boxes;
    delete []pred;
    delete []pred_per_class;
    delete []keep;
    delete []sorted_pred_cls;
    delete []data_buf;
    return detections;
}

// transform [x y w h angle] to [point1 point2 point3 point4]
std::vector<cv::Point> ShipDetector::computeLocation(float cx, float cy, float h, float w, float angle)
{
    std::vector<cv::Point> points;

    //std::cout<<"rato : w/h : " << w << "/" << h << "=" << w/h << " , and x : " << cx << ",y : " << cy << std::endl;

    float Xlt,Ylt,Xrt,Yrt,Xlb,Ylb,Xrb,Yrb;
    float cos_an,sin_an;

    float temp1,temp2;
    cv::Point point1,point2,point3,point4;

    Xlt = cx - w/2;
    Ylt = cy - h/2;
    Xrt = cx + w/2;
    Yrt = cy - h/2;
    Xlb = cx - w/2;
    Ylb = cy + h/2;
    Xrb = cx + w/2;
    Yrb = cy + h/2;

    angle = -angle;
    cos_an = cos(angle * PI / 180);
    sin_an = sin(angle * PI / 180);

    // �м���
    temp1 = -cx * cos_an + cy * sin_an + cx;
    temp2 = -cx * sin_an - cy * cos_an + cy;

    point1.x = Xlt * cos_an - Ylt * sin_an + temp1;
    point1.y = Xlt * sin_an + Ylt * cos_an + temp2;
    point2.x = Xrt * cos_an - Yrt * sin_an + temp1;
    point2.y = Xrt * sin_an + Yrt * cos_an + temp2;
    point3.x = Xrb * cos_an - Yrb * sin_an + temp1;
    point3.y = Xrb * sin_an + Yrb * cos_an + temp2;
    point4.x = Xlb * cos_an - Ylb * sin_an + temp1;
    point4.y = Xlb * sin_an + Ylb * cos_an + temp2;
    points.push_back(point1);
    points.push_back(point2);
    points.push_back(point3);
    points.push_back(point4);
    return points;
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  vis_detections
 *  Description:  Visuallize the detection result
 * =====================================================================================
 */
void ShipDetector::vis_detections(cv::Mat image, int* keep, int num_out, float* sorted_pred_cls, float CONF_THRESH,int class_label)
{
    int i=0;
    while(sorted_pred_cls[keep[i]*6+5]>CONF_THRESH && i < num_out)
    {
        if(i>=num_out)
            return;

        std::vector<cv::Point> points = computeLocation(sorted_pred_cls[keep[i]*6+0], sorted_pred_cls[keep[i]*6+1], sorted_pred_cls[keep[i]*6+3] , sorted_pred_cls[keep[i]*6+2], sorted_pred_cls[keep[i]*6+4]);
        if(class_label == 1) {
            cv::line(image,points[0],points[1],cv::Scalar(0,0,255),2);
            cv::line(image,points[1],points[2],cv::Scalar(0,0,255),2);
            cv::line(image,points[2],points[3],cv::Scalar(0,0,255),2);
            cv::line(image,points[3],points[0],cv::Scalar(0,0,255),2);
            //std::cout<<"class:"<<class_label<<std::endl;
        } else if(class_label == 2) {
            cv::line(image,points[0],points[1],cv::Scalar(255,0,255),2);
            cv::line(image,points[1],points[2],cv::Scalar(255,0,255),2);
            cv::line(image,points[2],points[3],cv::Scalar(255,0,255),2);
            cv::line(image,points[3],points[0],cv::Scalar(255,0,255),2);
            //std::cout<<"class:"<<class_label<<std::endl;
        } else if(class_label == 3) {
            cv::line(image,points[0],points[1],cv::Scalar(0,255,255),2);
            cv::line(image,points[1],points[2],cv::Scalar(0,255,255),2);
            cv::line(image,points[2],points[3],cv::Scalar(0,255,255),2);
            cv::line(image,points[3],points[0],cv::Scalar(0,255,255),2);
            //std::cout<<"class:"<<class_label<<std::endl;
        }else if(class_label == 4) {
            cv::line(image,points[0],points[1],cv::Scalar(0,255,0),2);
            cv::line(image,points[1],points[2],cv::Scalar(0,255,0),2);
            cv::line(image,points[2],points[3],cv::Scalar(0,255,0),2);
            cv::line(image,points[3],points[0],cv::Scalar(0,255,0),2);
            //std::cout<<"class:"<<class_label<<std::endl;
        }

        i++;
    }
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  boxes_sort
 *  Description:  Sort the bounding box according score
 * =====================================================================================
 */
void ShipDetector::boxes_sort(const int num, const float* pred, float* sorted_pred)
{
    vector<Info> my;
    Info tmp;
    for (int i = 0; i< num; i++)
    {
        tmp.score = pred[i*6 + 5];
        tmp.head = pred + i*6;
        my.push_back(tmp);
    }
    std::sort(my.begin(), my.end(), compare);
    for (int i=0; i<num; i++)
    {
        for (int j=0; j<6; j++)
            sorted_pred[i*6+j] = my[i].head[j];
    }
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  bbox_transform_inv
 *  Description:  Compute bounding box regression value
 *  num : the number of the rois : 300
 *  box_deltas : the regression info of box location : 5 * 5 * 300   : info * class_num * box_num    format : [dx, dy, dw, dh, da]
 *  pred_cls : the predicted class score of each box of the each class : 5 * 300   : class_num * box_num
 *  boxes : rois to regress to get the predect box location : 5 * 300 (each box has fixed roi between different class) format : [cx, cy, h, w, angle]
 *
 *  output:
 *  pred : the predicted information : format :[ctr_x,ctr_y,w,h,angle,conf_pred] : 6 * 5 * 300   info * class_num * box_num
 * =====================================================================================
 */
void ShipDetector::bbox_transform_inv(int num, const float* box_deltas, const float* pred_cls,
        float* boxes, float* pred)
{
    float width, height, ctr_x, ctr_y, dx, dy, dw, dh, pred_ctr_x, pred_ctr_y, pred_w, pred_h;
    float angle,da,pred_a;
    for(int i=0; i< num; i++)
    {
        width = boxes[i*5+3];
        height = boxes[i*5+2];
        ctr_x = boxes[i*5+0];
        ctr_y = boxes[i*5+1];
        angle = boxes[i*5+4];
        for (int j=0; j< class_num; j++)
        {
            // box_deltas : ÿ5��һ�飬ÿclass_num��Ϊһ���߽�������ع顣��ÿ���߽���ж�Ӧ��n�����Ļع�ֵ����ÿ���߽���Ӧ�Ļع�ֵΪ5*class_num
            dx = box_deltas[(i*class_num+j)*5+0];
            dy = box_deltas[(i*class_num+j)*5+1];
            dw = box_deltas[(i*class_num+j)*5+2];
            dh = box_deltas[(i*class_num+j)*5+3];

            da = box_deltas[(i*class_num+j)*5+4];

            pred_ctr_x = ctr_x + width*dx;
            pred_ctr_y = ctr_y + height*dy;
            pred_w = width * exp(dw);
            pred_h = height * exp(dh);

            //TODO �ǶȵĻع�
            da = da * 1.0 / PI * 180;       // ת��Ϊ�Ƕ�
            pred_a = angle + da;

            // pred_angle[np.where(pred_angle<-45)] += 180
            // pred_angle[np.where(pred_angle>135)] -= 180
            if (pred_a > 135)
                pred_a -= 180;
            else if(pred_a < -45)
                pred_a += 180;

            pred[(j*num+i)*6+0] = pred_ctr_x;
            pred[(j*num+i)*6+1] = pred_ctr_y;
            pred[(j*num+i)*6+2] = pred_w;
            pred[(j*num+i)*6+3] = pred_h;
            pred[(j*num+i)*6+4] = pred_a;
            pred[(j*num+i)*6+5] = pred_cls[i*class_num+j];
        }
    }

}


