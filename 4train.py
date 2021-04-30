import keras
from keras import optimizers
from keras.layers import Input, Conv2D, MaxPool2D, Dense, Activation, Flatten, AveragePooling2D, regularizers, Dropout, \
    BatchNormalization, MaxPooling2D, add, SeparableConv2D, GlobalAveragePooling2D
from keras.models import Model, Sequential
from keras.optimizers import Adam
from keras.callbacks import EarlyStopping, ReduceLROnPlateau, ModelCheckpoint
from keras.regularizers import l2
from sklearn.model_selection import train_test_split
from keras.utils import to_categorical

import os
import numpy as np

weight_decay = 1e-4
# 36 convolutional layers are structured into 14 modules

x = np.load('./x.npy')
y = np.load('./y.npy')

x_train, x_test, y_train, y_test = train_test_split(x, y, test_size=0.2, random_state=60)

np.save("test_x", x_test)
np.save("test_y", y_test)

x_train = x_train / 128.0 - 1
y_train = to_categorical(y_train)

x_test = x_test / 128.0 - 1
y_test = to_categorical(y_test)


def entryflow(x, params, top=False):
    # modules 2-4,13
    # params is (3,)
    # top = true means module 2, don't use relu
    residual = Conv2D(params[0], (1, 1), strides=(2, 2), padding='same')(x)
    residual = BatchNormalization()(residual)
    if top:
        x = Activation('relu')(x)
    x = SeparableConv2D(params[1], (3, 3), padding='same')(x)
    x = BatchNormalization()(x)
    x = Activation('relu')(x)
    x = SeparableConv2D(params[2], (3, 3), padding='same')(x)
    x = BatchNormalization()(x)
    x = MaxPooling2D((3, 3), strides=(2, 2), padding='same')(x)
    x = add([x, residual])
    return x


def middleflow(x, params):
    # modules 5-12, params is int
    residual = x
    x = Activation('relu')(x)
    x = SeparableConv2D(params, (3, 3), padding='same')(x)
    x = BatchNormalization()(x)
    x = Activation('relu')(x)
    x = SeparableConv2D(params, (3, 3), padding='same')(x)
    x = BatchNormalization()(x)
    x = Activation('relu')(x)
    x = SeparableConv2D(params, (3, 3), padding='same')(x)
    x = BatchNormalization()(x)
    x = add([x, residual])
    return x


def exitflow(x, params):
    # modules 14 , params is (2,)
    x = SeparableConv2D(params[0], (3, 3), padding='same')(x)
    x = BatchNormalization()(x)
    x = Activation('relu')(x)
    x = SeparableConv2D(params[1], (3, 3), padding='same')(x)
    x = BatchNormalization()(x)
    x = Activation('relu')(x)
    return x


def xception():
    # modules 1
    _in = Input(shape=(32, 32, 3))
    x = Conv2D(32, (3, 3), strides=(2, 2), padding='same',
               kernel_initializer="he_normal", kernel_regularizer=regularizers.l2(weight_decay))(_in)
    x = BatchNormalization(momentum=0.9, epsilon=1e-5)(x)
    x = Activation('relu')(x)
    x = Conv2D(64, (3, 3), strides=(1, 1), padding='same',
               kernel_initializer="he_normal", kernel_regularizer=regularizers.l2(weight_decay))(x)
    x = BatchNormalization(momentum=0.9, epsilon=1e-5)(x)
    x = Activation('relu')(x)
    x = Conv2D(364, (3, 3), strides=(1, 1), padding='same',
               kernel_initializer="he_normal", kernel_regularizer=regularizers.l2(weight_decay))(x)
    x = BatchNormalization(momentum=0.9, epsilon=1e-5)(x)
    x = Activation('relu')(x)
    # module 5-12
    for _ in range(4):
        x = middleflow(x, 364)
    # module 13
    x = entryflow(x, (512, 364, 512), True)
    # module 14
    x = exitflow(x, (768, 1024))
    # output
    x = GlobalAveragePooling2D()(x)
    x = Dense(4, activation='softmax')(x)
    return Model(_in, x)


def resnet_block(inputs, num_filters=16, kernel_size=3, strides=1, activation='relu'):
    x = Conv2D(num_filters, kernel_size=kernel_size, strides=strides, padding='same',
               kernel_initializer='he_normal', kernel_regularizer=l2(1e-4))(inputs)
    x = BatchNormalization()(x)
    if (activation):
        x = Activation('relu')(x)
    return x


# 建一个20层的ResNet网络
def resnet(input_shape):
    inputs = Input(shape=input_shape)  # Input层，用来当做占位使用

    # 第一层
    x = resnet_block(inputs)
    print('layer1,xshape:', x.shape)
    # 第2~7层
    for i in range(6):
        a = resnet_block(inputs=x)
        b = resnet_block(inputs=a, activation='relu')
        x = keras.layers.add([x, b])
        x = Activation('relu')(x)
    # out：32*32*16
    # 第8~13层
    for i in range(6):
        if i == 0:
            a = resnet_block(inputs=x, strides=2, num_filters=32)
        else:
            a = resnet_block(inputs=x, num_filters=32)
        b = resnet_block(inputs=a, activation='relu', num_filters=32)
        if i == 0:
            x = Conv2D(32, kernel_size=3, strides=2, padding='same',
                       kernel_initializer='he_normal', kernel_regularizer=l2(1e-4))(x)
        x = keras.layers.add([x, b])
        x = Activation('relu')(x)
    # out:16*16*32
    # 第14~19层
    for i in range(6):
        if i == 0:
            a = resnet_block(inputs=x, strides=2, num_filters=64)
        else:
            a = resnet_block(inputs=x, num_filters=64)

        b = resnet_block(inputs=a, activation='relu', num_filters=64)
        if i == 0:
            x = Conv2D(64, kernel_size=3, strides=2, padding='same',
                       kernel_initializer='he_normal', kernel_regularizer=l2(1e-4))(x)
        x = keras.layers.add([x, b])  # 相加操作，要求x、b shape完全一致
        x = Activation('relu')(x)
    # out:8*8*64
    # 第20层
    x = AveragePooling2D(pool_size=2)(x)
    # out:4*4*64
    y = Flatten()(x)
    # out:1024
    outputs = Dense(4, activation='softmax',
                    kernel_initializer='he_normal')(y)

    # 初始化模型
    # 之前的操作只是将多个神经网络层进行了相连，通过下面这一句的初始化操作，才算真正完成了一个模型的结构初始化
    model = Model(inputs=inputs, outputs=outputs)
    return model


if __name__ == "__main__":
    if not (os.path.exists('./models')):
        os.mkdir("./models")
    model = resnet((32, 32, 3))
    model.summary()

    # set optimizer
    # sgd = optimizers.SGD(lr=.1, momentum=0.9, nesterov=True)
    # model.compile(loss='categorical_crossentropy', optimizer=sgd, metrics=['accuracy'])
    opt = Adam(lr=0.001)
    model.compile(optimizer=opt, loss='categorical_crossentropy', metrics=["acc"])

    early_stop = EarlyStopping(patience=20)
    reduce_lr = ReduceLROnPlateau(patience=15)
    save_weights = ModelCheckpoint("./models/model_{epoch:02d}_{val_acc:.4f}.h5",
                                   save_best_only=True, monitor='val_acc')
    callbacks = [save_weights, reduce_lr]
    model.fit(x_train, y_train, epochs=15, batch_size=32,
              validation_data=(x_test, y_test), callbacks=callbacks)
