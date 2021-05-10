'''readme
发送apriltag和ai识别部分是分开的
原理：把openart获取的一帧截下来，
然后从那张图片里进行框提取，
提取到框就进行识别，
然后将识别结果发送出去

print函数里面的东西只是为了供在电脑ide中运行时可通过终端查看结果

可能存在的问题：
识别apriltag和识别数字这些的功能的耦合性不强
怕识别数字和识别水果的模型的精度问题导致识别不到

改进方向：
把识别apriltag码和识别模型进一步融合，用if语句框起来，
不过由于设计的识别模型是四分类的，所以有点难
或者用条件限制发送动物和水果的发送，
再或者在下位机接收时设置条件过滤
'''




import sensor, image, time, math
import pyb
import os, nncu
from machine import UART
uart = UART(1, baudrate=115200)     # 初始化串口 波特率设置为115200 TX是B12 RX是B13


sensor.reset()
sensor.set_pixformat(sensor.RGB565) # 设置摄像头像素格式
sensor.set_framesize(sensor.QQVGA)  # 设置摄像头分辨率
sensor.set_brightness(1000)         # 设置摄像头亮度
sensor.skip_frames(time = 200)

clock = time.clock()

net_path = "_1s_model_11_0.9928_xxxx.nncu"   # 定义模型的路径
labels = [line.rstrip() for line in open("/sd/labels_number.txt")]  # 加载标签
net = nncu.load(net_path, load_to_fb=True)   # 加载模型

def family_name(tag):
    if(tag.family() == image.TAG16H5):
        return "TAG16H5"
    if(tag.family() == image.TAG25H7):
        return "TAG25H7"
    if(tag.family() == image.TAG25H9):
        return "TAG25H9"
    if(tag.family() == image.TAG36H10):
        return "TAG36H10"
    if(tag.family() == image.TAG36H11):
        return "TAG36H11"
    if(tag.family() == image.ARTOOLKIT):
        return "ARTOOLKIT"


while (True):
    clock.tick()

    img = sensor.snapshot()  # 获取一幅图像
    for tag in img.find_apriltags(families=image.TAG25H9):  # 指定apriltag类型为TAG25H9
        img.draw_rectangle(tag.rect(), color=(255, 0, 0))  # 找到apriltag后绘制矩形框，将apriltag框起来
        img.draw_cross(tag.cx(), tag.cy(), color=(0, 255, 0))  # 在apriltag中间绘制十字
        print_args = (family_name(tag), tag.id(), (180 * tag.rotation()) / math.pi) #打印二维码的id，还有二维码旋转角度
        # 打印apriltag的类型、ID、旋转角度
        print("Tag Family %s, Tag ID %d, rotation %f (degrees)" % print_args)
    print(clock.fps())
    for r in img.find_rects(threshold = 50000):             # 在图像中搜索矩形
        img.draw_rectangle(r.rect(), color = (255, 0, 0))   # 绘制矩形外框，便于在IDE上查看识别到的矩形位置
        img1 = img.copy(r.rect())                           # 拷贝矩形框内的图像
        # 将矩形框内的图像使用训练好的模型进行分类
        # net.classify()将在图像的roi上运行网络(如果没有指定roi，则在整个图像上运行)
        # 将为每个位置生成一个分类得分输出向量。
        # 在每个比例下，检测窗口都以x_overlap（0-1）和y_overlap（0-1）为指导在ROI中移动。
        # 如果将重叠设置为0.5，那么每个检测窗口将与前一个窗口重叠50%。
        # 请注意，重叠越多，计算工作量就越大。因为每搜索/滑动一次都会运行一下模型。
        # 最后，对于在网络沿x/y方向滑动后的多尺度匹配，检测窗口将由scale_mul（0-1）缩小到min_scale（0-1）。
        # 下降到min_scale(0-1)。例如，如果scale_mul为0.5，则检测窗口将缩小50%。
        # 请注意，如果x_overlap和y_overlap较小，则在较小的比例下可以搜索更多区域...

        # 默认设置只是进行一次检测...更改它们以搜索图像...
        for obj in nncu.classify(net , img1, min_scale=1.0, scale_mul=0.5, x_overlap=0.0, y_overlap=0.0):
            print("**********\nTop 1 Detections at [x=%d,y=%d,w=%d,h=%d]" % obj.rect())
            sorted_list = sorted(zip(labels, obj.output()), key = lambda x: x[1], reverse = True)
            # 打印准确率最高的结果
            for i in range(1):
                print("%s = %f" % (sorted_list[i][0], sorted_list[i][1]))
    if tag.id:#如果识别到apriltag且id被赋值了，就发送id的字符串
        uart.write('%s',tag.id)
    if sorted_list:#如果识别到分类模型中的东西，就发送列表，列表第一项是分类结果，第二项是精度
        uart.write(bytearray(sorted_list))