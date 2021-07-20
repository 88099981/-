import nncu
import os
# from enum import Enum

import image
import math
import pyb
import sensor
import time
from machine import UART
from pyb import LED  # 导入LED

sensor.reset()  # 初始化相机传感器。
sensor.set_pixformat(sensor.RGB565)  # 设置相机模块的像素模式。 彩色三通道图必须16位
sensor.set_framesize(sensor.QQVGA)  # 设置相机模块的帧大小。
sensor.set_brightness(2800)  # 设置相机图像亮度。-3至+3。
sensor.skip_frames(time=200)  # 让相机图像在改变相机设置后稳定下来
sensor.set_auto_gain(False)  # 若您想追踪颜色，则需关闭白平衡。
sensor.set_auto_whitebal(False)  # 若您想追踪颜色，则需关闭白平衡。
clock = time.clock()

# red = LED(1)
# green = LED(2)
# red = LED(1)  # 定义一个LED1   红灯
# green = LED(2)  # 定义一个LED2   绿灯
# blue = LED(3)  # 定义一个LED3   蓝灯
white = LED(4)  # 定义一个LED4   照明灯

check_out = True

uart = UART(1, baudrate=115200)  # 串口初始化

uart_array = [0 for x in range(6)]
uart_array[0] = 0xD8
uart_array[-1] = 0xEE
apriltag_flag = False
number_flag = False
animal_fruit_flag = False
verify_num = 0
verify_i = 0
i = 0


number_net_path = "_1s_model_08_0.9965_xxxx.nncu"  # 定义模型的路径
animal_fruit_net_path = "_1s_model_19_0.6264_xxxx.nncu"
number_labels = [line.rstrip() for line in open("/sd/labels_number.txt")]  # 加载标签
animal_fruit_labels = [line.rstrip() for line in open("/sd/labels_number.txt")]
number_net = nncu.load(number_net_path, load_to_fb=True)  # 加载模型
animal_fruit_net = nncu.load(animal_fruit_net_path, load_to_fb=True)


# def led_off():
#     red.off()
#     green.off()
#     blue.off()
#     white.off()


"""连续性验证，排除误识别"""


def continuity_verify(num):
    global verify_num
    global verify_i
    global apriltag_flag
    global number_flag
    global animal_fruit_flag
    if verify_num == num:
        verify_i += 1
    else:
        verify_num = num
        verify_i = 0

    if verify_i >= 3 and num in [1, 2]:  # 云台左转或者右转
        uart_array[1] = num  # 左或者右，左1右2
        # red.on()
        verify_i = 0
        while True:
            img = sensor.snapshot()
            uart_back_num = uart.any()  # 获取当前串口数据数量
            if uart_back_num:
                uart_back = uart.read(uart_back_num)  # 读取串口数据
                if check_out:
                    print(uart_back)
                if uart_back == b'\x0A':
                    break  # 0A为云台转向完成，0B为舵机转向完成, 0C为等待完成， 0D为打靶完成
            uart.write(bytearray(uart_array))

        if check_out:
            print('apriltag is over')
        # red.off()
        # 接下来执行识别动物或者水果
        count_animal = 0
        count_fruit = 0
        while True:
            img_copy = sensor.snapshot()
            if check_out:
                print('animal_fruit is begin')
            for r in img_copy.find_rects(threshold=30000):  # 在图像中搜索矩形
                if check_out:
                    img_copy.draw_rectangle(r.rect(), color=(255, 0, 0))  # 绘制矩形外框，便于在IDE上查看识别到的矩形位置
                img2 = img_copy.copy(r.rect())  # 拷贝矩形框内的图像
                m = int(r.x() + img2.width() / 2)
                # print("m=%d" % m)
                n = int(r.y() + img2.height() / 2)

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
                for obj in nncu.classify(animal_fruit_net, img2, min_scale=1.0, scale_mul=0.5, x_overlap=0.0,
                                         y_overlap=0.0):
                    # print("**********\nTop 1 Detections at [x=%d,y=%d,w=%d,h=%d]" % obj.rect())
                    sorted_list = sorted(zip(animal_fruit_labels, obj.output()), key=lambda x: x[1], reverse=True)
                    for i in range(1):
                        print("%s = %f" % (sorted_list[i][0], sorted_list[i][1]))
                    if sorted_list[0][0] in ['0', '1', '2', '3', '4']:
                        # green.on()
                        count_animal += 1
                        if check_out:
                            print(count_animal)
                        if count_animal >= 3:
                            uart_array[3] = 0x05
                            if check_out:
                                print('animal is begin')
                            while True:  # 等待延时完毕
                                img = sensor.snapshot()
                                uart_back_num = uart.any()  # 获取当前串口数据数量
                                if uart_back_num:
                                    uart_back = uart.read(uart_back_num)  # 读取串口数据
                                    if check_out:
                                        print(uart_back)
                                    if uart_back == b'\x0C' or uart_back == b'\x0D':
                                        break  # 1为云台转向完成，2为舵机转向完成, 3为等待完成， 4为打靶完成
                                uart.write(bytearray(uart_array))
                            if check_out:
                                print('animal is over')
                            # green.off()
                            apriltag_flag = False
                            uart_array[1] = 0  # apriltag置零
                            uart_array[3] = 0
                    else:
                        # green.on()
                        count_fruit += 1
                        if check_out:
                            print(count_fruit)
                        if count_fruit >= 3: #  水果计数三次
                            uart_array[3] = 0x06
                            if check_out:
                                print('fruit is begin')
                            while True:
                                img_1 = sensor.snapshot()
                                uart_back_num = uart.any()  # 获取当前串口数据数量
                                if uart_back_num:
                                    uart_back = uart.read(uart_back_num)  # 读取串口数据
                                    if check_out:
                                        print(uart_back)
                                    if uart_back == b'\x0C' or uart_back == b'\x0D':
                                        break  # 1为云台转向完成，2为舵机转向完成, 3为等待完成， 4为打靶完成
                                for r in img_1.find_rects(threshold=30000):  # 在图像中搜索矩形
                                    # img_!.draw_rectangle(r.rect(), color=(255, 0, 0))  # 绘制矩形外框，便于在IDE上查看识别到的矩形位置
                                    img_2 = img_1.copy(r.rect())  # 拷贝矩形框内的图像
                                    m = int(r.x() + img_2.width() / 2)
                                    # print("m=%d" % m)
                                    n = int(r.y() + img_2.height() / 2)
                                    img_1.draw_cross(m, n, color=(255, 0, 0))
                                    uart_array[4] = m
                                uart.write(bytearray(uart_array))
                            if check_out:
                                print('fruit is over')
                            # green.off()
                            apriltag_flag = False
                            uart_array[1] = 0  # apriltag置零
                            uart_array[3] = 0  # 水果判断置零
                            uart_array[4] = 0  # 误差置零
                    if uart_back == b'\x0C' or uart_back == b'\x0D':
                        break
            if uart_back == b'\x0C' or uart_back == b'\x0D':
                break

    elif verify_i >= 3 and num in [3, 4]:  # 舵机转向，3是左，4是右
        if check_out:
            print('number is begin')
        # blue.on()
        uart_array[2] = num  # 左或者右，左3右4
        verify_i = 0
        while True:  # 等待转向完毕
            img = sensor.snapshot()
            uart_back_num = uart.any()  # 获取当前串口数据数量
            if uart_back_num:
                uart_back = uart.read(uart_back_num)  # 读取串口数据
                if check_out:
                    print(uart_back)
                if uart_back == b'\x0B':
                    break  # 0A为云台转向完成，0B为舵机转向完成, 0C为等待完成， 0D为打靶完成
            uart.write(bytearray(uart_array))
        # blue.off()
        number_flag = False
        if check_out:
            print('number is begin')
        uart_array[2] = 0  # number置零
    else:
        img = sensor.snapshot()
        uart.write(bytearray(uart_array))  # 发送无意义的0000刷新下位机

def number_recognition():
    global i
    global apriltag_flag  # 是否进入apriltag识别
    global number_flag  # 是否进入数字识别
    if check_out:
        print('number_rec is begin')
    img = sensor.snapshot()
    temp = img.copy()
    # temp.binary([(50,140)],invert=False)#True
    #temp.to_grayscale()
    ## print(temp.format())

    #for r in temp.find_rects(threshold=7000):  # 在图像中搜索矩形
        #w = r.w()
        #h = r.h()
        #w1 = abs((w - h) / ((w + h) / 2))
        #rect_area = w * h
        ## print(rect_area)
        ## print(w1)
        #if rect_area < 1250 or rect_area > 18000:
            #continue
        #if w1 > 0.2:
            #continue
        #img.draw_rectangle(r.rect(), color=(0, 255, 0))  # 绘制矩形外框，便于在IDE上查看识别到的矩形位置
        #img1 = temp.copy(r.rect())

    for r in img.find_rects(roi=(45,30,70,60),threshold=8000):  # 在图像中搜索矩形
        img.draw_rectangle(r.rect(), color=(255, 0, 0))  # 绘制矩形外框，便于在IDE上查看识别到的矩形位置
        w = r.w()
        h = r.h()
        if(w/h>1.2 or w/h<0.8):
            continue
        rect_area = w * h
        if(rect_area<400 or rect_area>5652):
            continue
        print(w)
        print(h)
        img1 = img.copy(r.rect())  # 拷贝矩形框内的图像
        for obj in nncu.classify(number_net, img1, min_scale=1.0, scale_mul=0.5, x_overlap=0.0, y_overlap=0.0):
            sorted_list = sorted(zip(number_labels, obj.output()), key=lambda x: x[1], reverse=True)
            print(sorted_list[0][0])  # 打印准确率最高的结果
            if int(sorted_list[0][0]) % 2 != 0:
                continuity_verify(4)  # 数字为奇数车辆右转（4）
            else:
                continuity_verify(3)  # 数字为偶数车辆左转（3）

"""核心循环函数"""
while True:
    # global apriltag_flag # 是否进入apriltag识别
    # global number_flag # 是否进入数字识别
    img = sensor.snapshot()
    #white.on()
    number_recognition()
