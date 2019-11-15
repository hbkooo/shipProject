#include <stdio.h>  // for snprintf
#include <string>
#include <vector>
#include <math.h>
#include <fstream>
#include <boost/python.hpp>
#include "caffe/caffe.hpp"
#include "gpu_nms.hpp"
//#include "rotate_polygon_nms.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace caffe;
using namespace std;

#define max(a, b) (((a)>(b)) ? (a) :(b))
#define min(a, b) (((a)<(b)) ? (a) :(b))

#define PI 3.1415926
const int class_num=5;   // 类别数
const float reduce_scale = 1.4;  // C++ interface should be dived with predicted height and width  :  h /= reduce_scale;  w /= reduce_scale;


/*
 * ===  Class  ======================================================================
 *         Name:  Detector
 *  Description:  FasterRCNN CXX Detector
 * =====================================================================================
 */
class Detector {
public:
    Detector(const string& model_file, const string& weights_file);
    void Detection(const string& im_name);
    void bbox_transform_inv(const int num, const float* box_deltas, const float* pred_cls, float* boxes, float* pred, int img_height, int img_width);
    void vis_detections(cv::Mat image, int* keep, int num_out, float* sorted_pred_cls, float CONF_THRESH,int class_label);
    void boxes_sort(int num, const float* pred, float* sorted_pred);
    std::vector<cv::Point> computeLocation(float cx, float cy, float h, float w, float angle);
private:
    shared_ptr<Net<float> > net_;
    Detector(){}
};

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Detector
 *  Description:  Load the model file and weights file
 * =====================================================================================
 */
//load modelfile and weights
Detector::Detector(const string& model_file, const string& weights_file)
{
    net_ = shared_ptr<Net<float> >(new Net<float>(model_file, caffe::TEST));
    std::cout<<"net is loded..."<<endl;
    net_->CopyTrainedLayersFrom(weights_file);
}


//Using for box sort
struct Info
{
    float score;
    const float* head;
};
bool compare(const Info& Info1, const Info& Info2)
{
    return Info1.score > Info2.score;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Detect
 *  Description:  Perform detection operation
 *                 Warning the max input size should less than 1000*600
 * =====================================================================================
 */
//perform detection operation
//input image max size 1000*600
void Detector::Detection(const string& im_name)
{
    float CONF_THRESH = 0.9;
    float NMS_THRESH = 0.3;
    
    // 这两个值根据相应的lib/fast_rcnn/config.py中的TEST.SCALES和TEST.MAX_SIZE中得到
    const int  max_input_side=1700;    
    const int  min_input_side=1000;  // python is target_size : 1000

    cv::Mat cv_img = cv::imread(im_name);
    cv::Mat cv_new(cv_img.rows, cv_img.cols, CV_32FC3, cv::Scalar(0,0,0));
    if(cv_img.empty())
    {
        std::cout<<"Can not get the image file !"<<endl;
        return ;
    }
    int max_side = max(cv_img.rows, cv_img.cols);
    int min_side = min(cv_img.rows, cv_img.cols);

    float max_side_scale = float(max_side) / float(max_input_side);
    float min_side_scale = float(min_side) /float( min_input_side);
    float max_scale=max(max_side_scale, min_side_scale);

    float img_scale = 1;

    if(max_scale > 1)
    {
        img_scale = float(1) / max_scale;
    }

    int height = int(cv_img.rows * img_scale);
    int width = int(cv_img.cols * img_scale);
    int num_out;
    cv::Mat cv_resized;
    
    
    // ############## python ################
    float scale = (float)min_input_side / (float)min_side;
    if (scale * (float)max_side > (float)max_input_side)
    {
        scale = (float)max_input_side / (float)max_side;
    }
    height = int(cv_img.rows * scale);
    width = int(cv_img.cols * scale);
    img_scale = scale;
    std::cout<<"resized height : "<<height << ",width : "<<width<<std::endl;
    // ######################################
    
    

    std::cout<<"imagename "<<im_name<<endl;
    float im_info[3];
    float* data_buf = new float[height*width*3];
    float *boxes = NULL;
    float *pred = NULL;
    float *pred_per_class = NULL;
    float *sorted_pred_cls = NULL;
    int *keep = NULL;
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
    
    std::cout<<"height:"<<height<<",width:"<<width<<std::endl;
    
    //const float * img_data = net_->blob_by_name("data")->cpu_data();
    //std::cout<<"img_data got from net_->blob_by_name(\"data\")->cpu_data()" << std::endl;
    //for (int i = 0; i < 3*height*width; i++){
    //    if(i%(3*height) == 0) {
    //        std::cout<<std::endl<<std::endl;
    //    }
    //    std::cout<<img_data[i] << "  ";
   // }
    
    net_->Forward();
    bbox_delt = net_->blob_by_name("bbox_pred")->cpu_data();  // box_deltas : 每5个一组，每class_num组为一个边界框，用来回归
                                                              // python : shape (300,25) (num,5*class_num)
                                                              
    num = net_->blob_by_name("rois")->num();


    rois = net_->blob_by_name("rois")->cpu_data();            // rois 每6个一组，一组的后5个表示box的位置信息，第一个信息未知
                                                              // python : shape (300,6) 
                                                              
    pred_cls = net_->blob_by_name("cls_prob")->cpu_data();    // 每num_class一组，一共num组，即第一个边界框所有类的得分，第二个边界框所有类的得分，...
                                                              // python : shape (300,5)  ==> C++ : 5 + 5 + 5 + ... + 5
        
    boxes = new float[num*5];
    pred = new float[num*6*class_num];
    pred_per_class = new float[num*6];
    sorted_pred_cls = new float[num*6];
    keep = new int[num];
    
    std::cout<<"im_info[0] : "<< im_info[0] << endl;  // cv_resized.rows
    std::cout<<"im_info[1] : "<< im_info[1] << endl;  // cv_resized.cols
    std::cout<<"im_info[2] : "<< im_info[2] << endl;  // img_scale
    std::cout<<"rois num : "<< num << endl;
    std::cout<<"rois size : " << sizeof(rois) << " , " << sizeof(rois)/sizeof(const float) << endl;
    std::cout<<"img_scale : " << img_scale << endl;
    
    /*
    for (int i = 0; i < 300*25; i++ ) 
    {
        std::cout<< "bbox_delt[" << i << "] : " << bbox_delt[i]<<endl;
    }
    std::cout<< "bbox_delt[7500] : " << bbox_delt[300*25]<<endl;
    std::cout<< "bbox_delt[7501] : " << bbox_delt[300*25]<<endl;
    std::cout<< "bbox_delt[7502] : " << bbox_delt[300*25]<<endl;
    
    for (int i = 0; i < 300*6;i++)
    {
        std::cout<<"rois[" << i << "] : " << rois[i]<< endl;
    }
    std::cout<<"rois[1800] : " << rois[1800]<< endl;
    std::cout<<"rois[1801] : "<<rois[1801]<<endl;
    std::cout<<"rois[1802] : "<<rois[1802]<<endl;
    
    for (int i = 0; i < 1500; i++)
    {
        std::cout<<"pred_cls[" << i << "] : " << pred_cls[i] << endl;
    }
    std::cout<<"pred_cls[1500] : " << pred_cls[1500] << endl;
    std::cout<<"pred_cls[1501] : " << pred_cls[1501] << endl;
    std::cout<<"pred_cls[1502] : " << pred_cls[1502] << endl;
    */

    for (int n = 0; n < num; n++)
    {
        for (int c = 0; c < 5; c++)
        {
            // boxes以5个为一个边界框的坐标，对应的rois位置为1,2,3,4,5、7,8,9,10,11、13,14,15,16,17、...4
            // rois : 每5个为一组
            boxes[n*5+c] = rois[n*6+c+1] / img_scale;
            //std::cout<<"\nn:"<<n<<",c:"<<c<<endl;
            //std::cout<<"box"<<n*5+c<<":"<<boxes[n*5+c]<<endl;
        }
        boxes[n*5+4] *= img_scale;   // 角度和图片的尺度无关，所以要再回复原来的角度大小
    }
    
    /*
    std::cout<< "from rois get boxes:"<<endl;
    for ( int i = 0; i < 1500; i++)
    {
        std::cout<<"boxes[" << i << "] : " << boxes[i] << endl;
    }*/
    
    // 得到 pred : [ctr_x,ctr_y,w,h,angle,conf_pred] : 6 * 5 * 300 
    // 每6个(ctr_x,ctr_y,w,h,angle,conf_pred)一组，其中前num组为class[0]对应的num个边界框，再num组为class[1]对应的num个边界框...
    bbox_transform_inv(num, bbox_delt, pred_cls, boxes, pred, cv_img.rows, cv_img.cols);
   
    /*
    std::cout<<"get predections:(ctr_x  ctr_y  w  h angle  score)"<<endl;
    for (int i = 0; i < 5; i++)
    {
        std::cout<<"class " << i << " : " <<endl;
        for(int j=0; j < 300; j++)
        {
            std::cout<<"class " << i << ", box " << j << ", pred[" << (i*300+j)*6 << "] : " <<pred[(i*300+j)*6]<<"  " <<pred[(i*300+j)*6+1] <<"  " ;
            std::cout<<pred[(i*300+j)*6+2] <<"  " <<pred[(i*300+j)*6+3] <<"  " <<pred[(i*300+j)*6+4] <<"  " <<pred[(i*300+j)*6+5] << endl;
        }
    }*/
    
    cv::Mat img;
    cv::resize(cv_img, img, cv::Size(width, height));
    
    for (int i = 1; i < class_num; i ++)
    {
        for (int j = 0; j< num; j++)
        {
            for (int k=0; k<6; k++)
                pred_per_class[j*6+k] = pred[(i*num+j)*6+k];  // class[0]为背景background，所以将前num组删去，即从(num+1)组开始提取数据到pred_per_class中
            
            // 不知道为什么C++预测的结果宽和高比python预测的大1.4倍，所以宽和高要除以1.4
            pred_per_class[j*6+2] /= reduce_scale;
            pred_per_class[j*6+3] /= reduce_scale;
            
        }
        boxes_sort(num, pred_per_class, sorted_pred_cls);
        //_rotate_nms(keep, &num_out, sorted_pred_cls, num, 6, NMS_THRESH, 0);
        _nms(keep, &num_out, sorted_pred_cls, num, 6, NMS_THRESH, 0);
        vis_detections(cv_img, keep, num_out, sorted_pred_cls, CONF_THRESH,i);
        
        /*
        // test 
        for (int m = 0; m < num; m++) 
        {
            std::cout<<"keep["<<m<<"]"<<keep[m]<<endl;
            if(sorted_pred_cls[m*6+5] > 0.9) 
            {
                std::cout<< "\nconfidence : " << sorted_pred_cls[m*6+5] << ",and class label is " << i << endl;
                std::cout<< "ctr_x : " << sorted_pred_cls[m*6+0] << endl;
                std::cout<< "ctr_y : " << sorted_pred_cls[m*6+1] << endl;
                std::cout<< "w : " << sorted_pred_cls[m*6+2] << endl;
                std::cout<< "h : " << sorted_pred_cls[m*6+3] << endl;
                std::cout<< "angle : " << sorted_pred_cls[m*6+4] << endl;
                std::cout<<endl<<endl;
            }
        }
        std::cout<<"class " << i << endl;*/
        
    }

    cv::imwrite("vis.jpg",cv_img);
    delete []boxes;
    delete []pred;
    delete []pred_per_class;
    delete []keep;
    delete []sorted_pred_cls;
    delete []data_buf;

}

// 根据矩形框的中心点和宽高和角度得到矩形框的四个点
std::vector<cv::Point> Detector::computeLocation(float cx, float cy, float h, float w, float angle) 
{
    std::vector<cv::Point> points;
    
    std::cout<<"rato : w/h : " << w << "/" << h << "=" << w/h << " , and x : " << cx << ",y : " << cy << std::endl;
    
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

    // 中间结果
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
void Detector::vis_detections(cv::Mat image, int* keep, int num_out, float* sorted_pred_cls, float CONF_THRESH,int class_label)
{
    int i=0;
    while(sorted_pred_cls[keep[i]*6+5]>CONF_THRESH && i < num_out)
    {
        if(i>=num_out)
            return;
        std::cout<< "confidence : " << sorted_pred_cls[keep[i]*6+5]<<std::endl;
        
        std::vector<cv::Point> points = computeLocation(sorted_pred_cls[keep[i]*6+0], sorted_pred_cls[keep[i]*6+1], sorted_pred_cls[keep[i]*6+3] , sorted_pred_cls[keep[i]*6+2], sorted_pred_cls[keep[i]*6+4]);
        //std::cout<<"class:"<<sorted_pred_cls[keep[i]*6+4]<<std::endl;
        if(class_label == 1) {
            cv::line(image,points[0],points[1],cv::Scalar(0,0,255),2);
            cv::line(image,points[1],points[2],cv::Scalar(0,0,255),2);
            cv::line(image,points[2],points[3],cv::Scalar(0,0,255),2);
            cv::line(image,points[3],points[0],cv::Scalar(0,0,255),2);       
            std::cout<<"class:"<<class_label<<std::endl;
        } else if(class_label == 2) {
            cv::line(image,points[0],points[1],cv::Scalar(255,0,255),2);
            cv::line(image,points[1],points[2],cv::Scalar(255,0,255),2);
            cv::line(image,points[2],points[3],cv::Scalar(255,0,255),2);
            cv::line(image,points[3],points[0],cv::Scalar(255,0,255),2);       
            std::cout<<"class:"<<class_label<<std::endl;
        } else if(class_label == 3) {
            cv::line(image,points[0],points[1],cv::Scalar(0,255,255),2);
            cv::line(image,points[1],points[2],cv::Scalar(0,255,255),2);
            cv::line(image,points[2],points[3],cv::Scalar(0,255,255),2);
            cv::line(image,points[3],points[0],cv::Scalar(0,255,255),2);       
            std::cout<<"class:"<<class_label<<std::endl;
        }else if(class_label == 4) {
            cv::line(image,points[0],points[1],cv::Scalar(0,255,0),2);
            cv::line(image,points[1],points[2],cv::Scalar(0,255,0),2);
            cv::line(image,points[2],points[3],cv::Scalar(0,255,0),2);
            cv::line(image,points[3],points[0],cv::Scalar(0,255,0),2);       
            std::cout<<"class:"<<class_label<<std::endl;
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
void Detector::boxes_sort(const int num, const float* pred, float* sorted_pred)
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
 *  box_deltas : the regression info of box location : 5 * 5 * 300   : info * class_num * box_num   
 *  pred_cls : the predicted class score of each box of the each class : 5 * 300   : class_num * box_num
 *  boxes : rois to regress to get the predect box location : 5 * 300 (each box has fixed roi between different class)
 *  
 *  output:
 *  pred : the predicted information : format :[ctr_x,ctr_y,w,h,angle,conf_pred] : 6 * 5 * 300   info * class_num * box_num
 * =====================================================================================
 */
void Detector::bbox_transform_inv(int num, const float* box_deltas, const float* pred_cls, float* boxes, float* pred, int img_height, int img_width)
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
            // box_deltas : 每5个一组，每class_num组为一个边界框，用来回归。即每个边界框都有对应的n中类别的回归值，即每个边界框对应的回归值为5*class_num
            dx = box_deltas[(i*class_num+j)*5+0];
            dy = box_deltas[(i*class_num+j)*5+1];
            dw = box_deltas[(i*class_num+j)*5+2];
            dh = box_deltas[(i*class_num+j)*5+3];
            
            da = box_deltas[(i*class_num+j)*5+4];
            
            pred_ctr_x = ctr_x + width*dx;
            pred_ctr_y = ctr_y + height*dy;
            pred_w = width * exp(dw);
            pred_h = height * exp(dh);
            
            //TODO 角度的回归  
            da = da * 1.0 / PI * 180;       // 转换为角度         
            pred_a = angle + da;
            
            // pred_angle[np.where(pred_angle<-45)] += 180
	          // pred_angle[np.where(pred_angle>135)] -= 180
            if (pred_a > 135)
                pred_a -= 180;
            else if(pred_a < -45)
                pred_a += 180;
            
            // pred : 每6个(ctr_x,ctr_y,w,h,angle,conf_pred)一组，其中前num组为class[0]对应的num个边界框，再num组为class[1]对应的num个边界框...
            //pred[(j*num+i)*6+0] = max(min(pred_ctr_x - 0.5* pred_w, img_width -1), 0);
            //pred[(j*num+i)*6+1] = max(min(pred_ctr_y - 0.5* pred_h, img_height -1), 0);
            //pred[(j*num+i)*6+2] = max(min(pred_ctr_x + 0.5* pred_w, img_width -1), 0);
            //pred[(j*num+i)*6+3] = max(min(pred_ctr_y + 0.5* pred_h, img_height -1), 0);
            
            pred[(j*num+i)*6+0] = pred_ctr_x;
            pred[(j*num+i)*6+1] = pred_ctr_y;
            pred[(j*num+i)*6+2] = pred_w;
            pred[(j*num+i)*6+3] = pred_h;
            pred[(j*num+i)*6+4] = pred_a;
            
            pred[(j*num+i)*6+5] = pred_cls[i*class_num+j];
        }
    }

}

int main()
{
    //string model_file = "/home/lyh1/workspace/py-faster-rcnn/models/pascal_voc/VGG_CNN_M_1024/faster_rcnn_alt_opt/faster_rcnn_test.pt";
    //string weights_file = "/home/lyh1/workspace/py-faster-rcnn/output/default/yuanzhang_car/vgg_cnn_m_1024_fast_rcnn_stage2_iter_40000.caffemodel";
    string model_file = "/home/hbk/RRPN_new/models/rrpn/VGG16/faster_rcnn_end2end/test.prototxt";
    string weights_file = "/home/hbk/RRPN_new/data/faster_rcnn_models/vgg16_faster_rcnn_iter_60000.caffemodel";
    string image_file  ="/home/hbk/RRPN_new/data/demo/demo/ship_are.jpg";// ships_10741_1.jpg
    //image_file = "/home/hbk/data/image/dota/ships_9598.jpg";
    image_file = "/home/hbk/data/image/ship_are.jpg";
    
    std::cout<<"model_path:"<<model_file<<endl;
    std::cout<<"weights_path:"<<weights_file<<endl;
    std::cout<<"image_path:"<<image_file<<endl;
    
    int GPUID=0;
    Caffe::SetDevice(GPUID);
    Caffe::set_mode(Caffe::GPU);
    Detector det = Detector(model_file, weights_file);
    det.Detection(image_file);
return 0;
}
