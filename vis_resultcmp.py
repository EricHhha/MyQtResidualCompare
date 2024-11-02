import os
import random
import uuid

from tqdm import tqdm
import cv2
import numpy as np
import matplotlib.pyplot as plt
GT_Dir="results_compare/GT/"
Bicubic_dir="results_compare/Bicubic4x/"
Phase_dir ="results_compare/JiLin-1_ori_resSr/"
LR_Dir="results_compare/LR4x/"
MSDTGP_Dir="results_compare/MSDTGP/"
SAVE_Dir="results_compare/Save_Dir/"
# MSDTGP_Dir="/home/lenovo/Documents/code/PhaseVSRnet/visual_cmp/exp_compare/MSDTGP/"                                   #生成的EDVR效果图


CACHE_DATA = True
cache_data={}

#获取文件目录结构
def getFileDict():
    #获取全部场景
    scenes=os.listdir(GT_Dir)
    scene_files={}
    for scene in scenes:
        dir_scene=os.path.join(GT_Dir,scene)
        fileNames=os.listdir(dir_scene)
        scene_files[scene]=list(fileNames)
    return scene_files

#获取残差路径
def save_residual_img(dir,scene,file,mode="type"):
    ori_file = os.path.join(dir, scene, file)
    if "_residual" in scene:
        return 0
    if not os.path.exists(ori_file):
        return 0
    if mode=="phase":
        index = int(file.split(".")[0])+1
        ori_file = os.path.join(dir, scene, "rgb_sr_%02d.png" % index)
    residual_dir = os.path.abspath(os.path.join(dir,scene+"_residual",file))
    os.makedirs(os.path.join(dir,scene+"_residual"),exist_ok=True)
    gt_file = os.path.join(GT_Dir,scene,file)
    if ori_file in cache_data:
        ori_img = cache_data.get(ori_file)
    else:
        ori_img = cv2.imread(ori_file, cv2.IMREAD_GRAYSCALE)  # 以灰度图形式读入
        cache_data[ori_file] = ori_img
    if gt_file in cache_data:
        gt_img = cache_data.get(gt_file)
    else:
        gt_img = cv2.imread(gt_file, cv2.IMREAD_GRAYSCALE)  # 以灰度图形式读入
        cache_data[gt_file] = gt_img
    if ori_img is None:
        ccc=0
    if gt_img is None:
        ccc=0
    res_img = ori_img-gt_img
    # cv2.imshow("aa",res_img)
    # cv2.waitKey(0)
    # plt.imshow(res_img,cmap='gray')
    # plt.show()
    cv2.imwrite(residual_dir,res_img)
    # plt.savefig(residual_dir)
    return residual_dir,gt_file


#生成残差效果图
def product_residual(scene_dict):
    for scene,files in scene_dict.items():
        #生成Bicubic的残差结果
        for file_name in tqdm(files):
            #保存bicubic残差结果
            save_residual_img(Bicubic_dir,scene,file_name,"bic")
            save_residual_img(GT_Dir,scene,file_name,"GT")
            save_residual_img(Phase_dir,scene,file_name,"phase")
            # save_residual_img(LR_Dir,scene,file_name,"LR")
            save_residual_img(MSDTGP_Dir,scene,file_name,"MSDTGP")
            # save_residual_img(MSDTGP_Dir, scene, file_name)

#画矩形框
def drawBox(img,bbox):
    img_c=img.copy()
    cv2.rectangle(img_c,(bbox[0],bbox[1]),(bbox[2],bbox[3]),(0,255,0),2)
    return img_c
def drwaPicture(img_src,img_target,bbox):
    plt.imshow(img_src)
    plt.show()
    plt.imshow(img_target)
    plt.show()
    img_c=img_src.copy()
    # img_c[bbox[0]:bbox[2],bbox[1]:bbox[3],:]=img_target[:,:,:]
    img_c[bbox[0]:bbox[2],bbox[1]:bbox[3]]=img_target
    plt.imshow(img_c)
    plt.show()
    # cv2.rectangle(img_c,(bbox[0],bbox[1]),(bbox[2],bbox[3]),(0,255,0),1.0)
    return img_c

#刷新窗口
def refreshWindow(LR_img,phase_img,bic_img,gt_img,msdtgp_img,bbox,x=0,y=0,window_name="aa"):
    w,h=gt_img.shape[:2]
    #更新窗口显示信息
    #LR_img
    # plt.imshow(gt_img)
    # plt.show()
    PhaseImage_box=drawBox(phase_img,bbox)
    BicubicImage_box=drawBox(bic_img,bbox)
    GTImage_box=drawBox(gt_img,bbox)
    MSDTGPImage_box=drawBox(msdtgp_img,bbox)

    GT_imgc=drawBox(gt_img,bbox)    #画完框的LR
    GT_imgc_or=GT_imgc.copy()
    # cv2.circle(GT_imgc,(x,y),3,(0,0,255),1)
    # plt.imshow(GT_imgc)
    # plt.show()
    #取框中信息
    # plt.imshow(phase_img)
    # plt.show()
    phase_imgc = phase_img[bbox[1]:bbox[3],bbox[0]:bbox[2],:]
    # plt.imshow(phase_imgc)
    # plt.show()
    bic_imgc = bic_img[bbox[1]:bbox[3],bbox[0]:bbox[2],:]
    gt_imgc = gt_img[bbox[1]:bbox[3],bbox[0]:bbox[2],:]
    msdtgp_imgc = msdtgp_img[bbox[1]:bbox[3],bbox[0]:bbox[2],:]
    # plt.imshow(phase_imgc)
    # plt.show()
    LR_imgcu = np.zeros_like(LR_img)
    phase_imgcu = np.zeros_like(LR_img)
    bic_imgcu = np.zeros_like(LR_img)
    gt_imgcu = np.zeros_like(LR_img)
    msdtgp_imgcu = np.zeros_like(LR_img)

    phase_imgcu = cv2.resize(phase_imgc,(int(gt_img.shape[0] // 2), int(gt_img.shape[1] // 2)),phase_imgcu)
    bic_imgcu = cv2.resize(bic_imgc,(int(gt_img.shape[0] // 2), int(gt_img.shape[1] // 2)),bic_imgcu)
    gt_imgcu = cv2.resize(gt_imgc,(int(gt_img.shape[0] // 2), int(gt_img.shape[1] // 2)),gt_imgcu)
    msdtgp_imgcu = cv2.resize(msdtgp_imgc,(int(gt_img.shape[0] // 2), int(gt_img.shape[1] // 2)),msdtgp_imgcu)
    # phase_imgcu = cv2.resize(phase_imgc,(int(gt_img.shape[0] // 2), int(gt_img.shape[1] // 2)),phase_imgcu)

    # plt.imshow(phase_imgcu)
    # plt.show()


    # cv2.pyrUp(phase_imgc,phase_imgcu,(int(gt_img.shape[0] // 2), int(gt_img.shape[1] // 2)))
    # cv2.pyrUp(bic_imgc,bic_imgcu,(int(gt_img.shape[0] // 2), int(gt_img.shape[1] // 2)))
    # cv2.pyrUp(gt_imgc,gt_imgcu,(int(gt_img.shape[0] // 2), int(gt_img.shape[1] // 2)))
    # cv2.pyrUp(msdtgp_imgc,msdtgp_imgcu,(int(gt_img.shape[0] // 2), int(gt_img.shape[1] // 2)))
    #h w c
    # img_back=np.ones((gt_img.shape[0],gt_img.shape[1]*3,3)) * 255.0
    #
    # img_show=drwaPicture(img_back,GT_imgc,[0,0,w,h])

    #求灰度值
    phase_imgcu_gray=cv2.cvtColor(phase_imgcu,cv2.COLOR_RGB2GRAY)
    bic_imgcu_gray=cv2.cvtColor(bic_imgcu,cv2.COLOR_RGB2GRAY)
    gt_imgcu_gray=cv2.cvtColor(gt_imgcu,cv2.COLOR_RGB2GRAY)
    msdtgp_imgcu_gray=cv2.cvtColor(msdtgp_imgcu,cv2.COLOR_RGB2GRAY)
    # phase_imgcu_gray=cv2.cvtColor(phase_imgcu,cv2.COLOR_RGB2GRAY)

    #求差值
    phase_imgcu_residual = phase_imgcu_gray - gt_imgcu_gray
    bic_imgcu_residual = bic_imgcu_gray - gt_imgcu_gray
    gt_imgcu_residual = gt_imgcu_gray - gt_imgcu_gray
    msdtgp_imgcu_residual = msdtgp_imgcu_gray - gt_imgcu_gray

    min_phase,max_phase=np.min(phase_imgcu_residual),np.max(phase_imgcu_residual)
    #范围是【125，255】
    # phase_imgcu_residual = 125.0 + (phase_imgcu_residual- min_phase )*((255.0-125.0)/(max_phase-min_phase))
    phase_imgcu_residual = 0.0 + (phase_imgcu_residual- min_phase )*((255.0-0.0)/(max_phase-min_phase))
    #其他的
    min_phase,max_phase=np.min(bic_imgcu_residual),np.max(bic_imgcu_residual)
    #范围是【125，255】
    # phase_imgcu_residual = 125.0 + (phase_imgcu_residual- min_phase )*((255.0-125.0)/(max_phase-min_phase))
    bic_imgcu_residual = 0.0 + (bic_imgcu_residual- min_phase )*((255.0-0.0)/(max_phase-min_phase))

    # min_phase,max_phase=np.min(gt_imgcu_residual),np.max(gt_imgcu_residual)
    #范围是【125，255】
    # phase_imgcu_residual = 125.0 + (phase_imgcu_residual- min_phase )*((255.0-125.0)/(max_phase-min_phase))
    # gt_imgcu_residual = 0.0 + (gt_imgcu_residual- min_phase )*((255.0-0.0)/(max_phase-min_phase))

    min_phase,max_phase=np.min(msdtgp_imgcu_residual),np.max(msdtgp_imgcu_residual)
    #范围是【125，255】
    # phase_imgcu_residual = 125.0 + (phase_imgcu_residual- min_phase )*((255.0-125.0)/(max_phase-min_phase))
    msdtgp_imgcu_residual = 0.0 + (msdtgp_imgcu_residual- min_phase )*((255.0-0.0)/(max_phase-min_phase))

    # # phase_imgcu_residual = ((phase_imgcu_gray - gt_imgcu_gray)/255)*130+125
    # bic_imgcu_residual = (np.abs(bic_imgcu_gray - gt_imgcu_gray)/255)*125+130
    # # gt_imgcu_residual = (np.abs(gt_imgcu_gray - gt_imgcu_gray)/255)*105+150
    # msdtgp_imgcu_residual = (np.abs(msdtgp_imgcu_gray - gt_imgcu_gray)/255)*120+135
    #
    # gt_imgcu_residual = (np.abs(gt_imgcu_gray - gt_imgcu_gray)/255)*105 +125

    # phase_imgcu_residual = np.abs(phase_imgcu_gray - gt_imgcu_gray)
    # 灰度图转伪彩色
    # COLORMAP_AUTUMN、COLORMAP_BONE、COLORMAP_JET、COLORMAP_WINTER、COLORMAP_RAINBOW、COLORMAP_OCEAN、COLORMAP_SUMMER: int
    # COLORMAP_SPRING、COLORMAP_COOL、COLORMAP_HSV、COLORMAP_PINK、COLORMAP_HOT、COLORMAP_PARULA、COLORMAP_MAGMA、COLORMAP_INFERNO: int
    # COLORMAP_PLASMA、COLORMAP_VIRIDIS、COLORMAP_CIVIDIS、COLORMAP_TWILIGHT、COLORMAP_TWILIGHT_SHIFTED、COLORMAP_TURBO、
    # COLORMAP_DEEPGREEN、ColormapTypes = int


    phase_imgcu_residual_show = cv2.applyColorMap(phase_imgcu_residual.astype(np.uint8), cv2.COLORMAP_JET)
    # phase_imgcu_residual_show = cv2.cvtColor( phase_imgcu_residual_show, cv2.COLOR_BGR2RGB)

    bic_imgcu_residual_show = cv2.applyColorMap(bic_imgcu_residual.astype(np.uint8), cv2.COLORMAP_JET)
    # bic_imgcu_residual_show = cv2.cvtColor( bic_imgcu_residual_show, cv2.COLOR_BGR2RGB)

    gt_imgcu_residual_show = cv2.applyColorMap(gt_imgcu_residual.astype(np.uint8), cv2.COLORMAP_JET)
    # gt_imgcu_residual_show = cv2.cvtColor( gt_imgcu_residual_show, cv2.COLOR_BGR2RGB)

    msdtgp_imgcu_residual_show = cv2.applyColorMap(msdtgp_imgcu_residual.astype(np.uint8), cv2.COLORMAP_JET)
    # msdtgp_imgcu_residual_show = cv2.cvtColor( msdtgp_imgcu_residual_show, cv2.COLOR_BGR2RGB)

    # phase_imgcu_residual_show = np.array([phase_imgcu_residual for i in range(3)]).transpose(2,1,0)
    # bic_imgcu_residual_show = np.array([bic_imgcu_residual for i in range(3)]).transpose(2,1,0)
    # gt_imgcu_residual_show = np.array([gt_imgcu_residual for i in range(3)]).transpose(2,1,0)
    # msdtgp_imgcu_residual_show = np.array([msdtgp_imgcu_residual for i in range(3)]).transpose(2,1,0)
    # # phase_imgcu_residual_show = np.array([phase_imgcu_residual for i in range(3)]).transpose(1,2,0)

    # bic_imgcu_residual = np.abs(bic_imgcu_gray - gt_imgcu_gray)
    # gt_imgcu_residual = np.abs(gt_imgcu_gray - gt_imgcu_gray)
    # msdtgp_imgcu_residual = np.abs(msdtgp_imgcu_gray - gt_imgcu_gray)
    #画图
    # phase_imgcu_residual_show = cv2.addWeighted(phase_imgcu, 1.0, phase_imgcu, 0.3, 0.3)
    # cv2.imwrite(os.path.join(SAVE_Dir,"imgcu.png"),phase_imgcu)
    # cv2.imwrite(os.path.join(SAVE_Dir,"imgres.png"),phase_imgcu_residual_show)
    phase_imgcu_residual_show_noadd = phase_imgcu_residual_show.copy()
    bic_imgcu_residual_show_noadd = bic_imgcu_residual_show.copy()
    gt_imgcu_residual_show_noadd = gt_imgcu_residual_show.copy()
    msdtgp_imgcu_residual_show_noadd = msdtgp_imgcu_residual_show.copy()

    phase_imgcu_residual_show = 0.1 * phase_imgcu_residual_show + phase_imgcu
    # cv2.imwrite(os.path.join(SAVE_Dir,"fusion.png"),phase_imgcu_residual_show)
    bic_imgcu_residual_show = 0.1 * bic_imgcu_residual_show + bic_imgcu
    # gt_imgcu_residual_show = 0.15 * gt_imgcu_residual_show + gt_imgcu
    gt_imgcu_residual_show = gt_imgcu
    msdtgp_imgcu_residual_show = 0.1 * msdtgp_imgcu_residual_show + msdtgp_imgcu


    img2=np.concatenate([phase_imgcu,phase_imgcu_residual_show.astype(np.uint8)],axis=0)
    img3=np.concatenate([bic_imgcu,bic_imgcu_residual_show.astype(np.uint8)],axis=0)
    img4=np.concatenate([gt_imgcu,gt_imgcu_residual_show.astype(np.uint8)],axis=0)
    img5=np.concatenate([msdtgp_imgcu,msdtgp_imgcu_residual_show.astype(np.uint8)],axis=0)
    # img5=np.concatenate([phase_imgcu,msdtgp_imgcu_residual],axis=0)
    img_show= np.concatenate([GT_imgc_or,img4,img3,img5,img2],axis=1)
    cv2.imwrite(os.path.join(SAVE_Dir,"fusion_all.png"),img_show)
    cv2.imshow(window_name,img_show)
    while(True):
        if cv2.waitKey(1) & 0xFF == ord('p'):  # 摁下q退出
            break
        if cv2.waitKey(1) & 0xFF == ord('s'):
            savePics(img_show,
                phase_imgcu_residual_show_noadd,bic_imgcu_residual_show_noadd, gt_imgcu_residual_show_noadd,msdtgp_imgcu_residual_show_noadd,
                PhaseImage_box,BicubicImage_box,GTImage_box,MSDTGPImage_box,
                LR_img,GT_imgc_or,GT_imgc,
                phase_imgcu,bic_imgcu,gt_imgcu,msdtgp_imgcu,
                phase_imgcu_residual_show,bic_imgcu_residual_show,gt_imgcu_residual_show,msdtgp_imgcu_residual_show)

    cv2.destroyAllWindows()
    # plt.imshow(img_show)
    # plt.show()
    # ccc=0
    # PhaseImage_box=drawBox(phase_img)
    # BicubicImage_box=drawBox(bic_img)
    # GTImage_box=drawBox(gt_img)
    # MSDTGPImage_box=drawBox(msdtgp_img)
def savePics(img_show,phase_imgcu_residual_show_noadd,bic_imgcu_residual_show_noadd, gt_imgcu_residual_show_noadd,msdtgp_imgcu_residual_show_noadd,
    PhaseImage_box,BicubicImage_box,GTImage_box,MSDTGPImage_box,
    LR_img,GT_imgc_or,GT_imgc,phase_imgcu,bic_imgcu,gt_imgcu,msdtgp_imgcu,
    phase_imgcu_residual_show,bic_imgcu_residual_show,gt_imgcu_residual_show,msdtgp_imgcu_residual_show):
    save_dir=os.path.join(SAVE_Dir,str(uuid.uuid4())[:8])
    if not os.path.exists(save_dir):
        os.makedirs(save_dir, exist_ok=True)
    cv2.imwrite(os.path.join(save_dir, "a_image_overview.png"), img_show)
    cv2.imwrite(os.path.join(save_dir, "phase_imgcu_residual_show_noadd.png"), phase_imgcu_residual_show_noadd)
    cv2.imwrite(os.path.join(save_dir, "bic_imgcu_residual_show_noadd.png"), bic_imgcu_residual_show_noadd)
    cv2.imwrite(os.path.join(save_dir, "gt_imgcu_residual_show_noadd.png"), gt_imgcu_residual_show_noadd)
    cv2.imwrite(os.path.join(save_dir, "msdtgp_imgcu_residual_show_noadd.png"), msdtgp_imgcu_residual_show_noadd)

    cv2.imwrite(os.path.join(save_dir,"Phase_imgbox_whole.png"),PhaseImage_box)
    cv2.imwrite(os.path.join(save_dir,"Bicubic_imgbox_whole.png"),BicubicImage_box)
    cv2.imwrite(os.path.join(save_dir,"GT_imgbox_whole.png"),GTImage_box)
    cv2.imwrite(os.path.join(save_dir,"MSDTGP_imgbox_whole.png"),MSDTGPImage_box)

    cv2.imwrite(os.path.join(save_dir,"LR_img.png"),LR_img)
    cv2.imwrite(os.path.join(save_dir,"GT_img_box.png"),GT_imgc_or)
    cv2.imwrite(os.path.join(save_dir,"GT_img_boxpt.png"),GT_imgc)
    cv2.imwrite(os.path.join(save_dir,"phasevsr_img_area.png"),phase_imgcu)
    cv2.imwrite(os.path.join(save_dir,"bicubic_img_area.png"),bic_imgcu)
    cv2.imwrite(os.path.join(save_dir,"GT_img_area.png"),gt_imgcu)
    cv2.imwrite(os.path.join(save_dir,"msdtgp_img_area.png"),msdtgp_imgcu)
    cv2.imwrite(os.path.join(save_dir,"phasevsr_img_resdiual.png"),phase_imgcu_residual_show)
    cv2.imwrite(os.path.join(save_dir,"bicubic_img_resdiual.png"),bic_imgcu_residual_show)
    cv2.imwrite(os.path.join(save_dir,"GT_img_resdiual.png"),gt_imgcu_residual_show)
    cv2.imwrite(os.path.join(save_dir,"msdtgp_img_resdiual.png"),msdtgp_imgcu_residual_show)
    print(f"save picture at: {save_dir}")
    pass



# 回调函数
def MouseEvent(event, x, y, flags, param):
    # print('[{},{}]'.format(x, y))  # 坐标，原点在左上角
    if flags == cv2.EVENT_FLAG_SHIFTKEY:
        print("shift 键按下")
        if event == cv2.EVENT_MOUSEMOVE:
            LR_img, phase_img, bic_img, gt_img, msdtgp_img, box_size, windowname= param
            print(f"move x:{x} y:{y} box_size={box_size}\n")
            bbox=[x-box_size,y-box_size,x+box_size,y+box_size]
            print(f"bbox:{bbox[0]},{bbox[1]},{bbox[2]},{bbox[3]}")
            refreshWindow(LR_img,phase_img,bic_img,gt_img,msdtgp_img,bbox,x,y,windowname)
    if event == cv2.EVENT_LBUTTONDBLCLK:
        print(f"左键双击")
        LR_img, phase_img, bic_img, gt_img, msdtgp_img, box_size, windowname = param
        print(f"move x:{x} y:{y} box_size={box_size}\n")
        bbox = [x - box_size, y - box_size, x + box_size, y + box_size]
        print(f"bbox:{bbox[0]},{bbox[1]},{bbox[2]},{bbox[3]}")
        refreshWindow(LR_img, phase_img, bic_img, gt_img, msdtgp_img, bbox, x, y, windowname)
    '''
    if flags == cv2.EVENT_FLAG_ALTKEY:
        print('摁住Alt')
    if flags == cv2.EVENT_FLAG_CTRLKEY:
        print('摁住Ctrl')
    if flags == cv2.EVENT_FLAG_SHIFTKEY:
        print('摁住Shift')

    if flags == cv2.EVENT_FLAG_LBUTTON:
        print('摁住左键')
    if flags == cv2.EVENT_FLAG_MBUTTON:
        print('摁住中键')
    if flags == cv2.EVENT_FLAG_RBUTTON:
        print('摁住右键')
    '''

    # if event == cv2.EVENT_LBUTTONDBLCLK:
    #     print('左键双击')
    # if event == cv2.EVENT_MBUTTONDBLCLK:
    #     print('中键双击')
    # if event == cv2.EVENT_RBUTTONDBLCLK:
    #     print('右键双击')

    '''
    if event == cv2.EVENT_LBUTTONDOWN:
        print('左键击下')
    if event == cv2.EVENT_LBUTTONUP:
        print('左键弹起')
    if event == cv2.EVENT_MBUTTONDOWN:
        print('中键击下')
    if event == cv2.EVENT_MBUTTONUP:
        print('中键弹起')
    if event == cv2.EVENT_RBUTTONDOWN:
        print('右键击下')
    if event == cv2.EVENT_RBUTTONUP:
        print('右键弹起')
    '''

    # if event == cv2.EVENT_MOUSEWHEEL:
    #     if flags > 0:
    #         print('向前滚动')
    #     else:
    #         print('向后滚动')
    # if event == cv2.EVENT_MOUSEHWHEEL:
    #     if flags > 0:
    #         print('向左滚动')  # 按住Alt
    #     else:
    #         print('向右滚动')

#choice window
def choice_Windows(scene,file):
    LR_file = os.path.join(LR_Dir, scene, file)                                     #LR file
    if not os.path.exists(LR_file):
        return 0
    index = int(file.split(".")[0])+1
    phase_file = os.path.join(Phase_dir, scene, "rgb_sr_%02d.png" % index)               #phase file
    bic_file = os.path.join(Bicubic_dir, scene, file)
    gt_file = os.path.join(GT_Dir, scene, file)
    msdtgp_file = os.path.join(MSDTGP_Dir, scene, file)

    LR_img = cv2.imread(LR_file)
    phase_img = cv2.imread(phase_file)
    bic_img = cv2.imread(bic_file)
    gt_img = cv2.imread(gt_file)
    msdtgp_img = cv2.imread(msdtgp_file)
    # phase_img = cv2.imread(phase_file)[:,:,::-1]

    # LR_img = cv2.imread(LR_file)[:,:,::-1]
    # phase_img = cv2.imread(phase_file)[:,:,::-1]
    # bic_img = cv2.imread(bic_file)[:,:,::-1]
    # gt_img = cv2.imread(gt_file)[:,:,::-1]
    # msdtgp_img = cv2.imread(msdtgp_file)[:,:,pp::-1]
    # # phase_img = cv2.imread(phase_file)[:,:,::-1]

    windowname= scene
    cv2.namedWindow(windowname,cv2.WINDOW_NORMAL)
    cv2.setMouseCallback(windowname,MouseEvent,(LR_img,phase_img,bic_img,gt_img,msdtgp_img,40,windowname))
    refreshWindow(LR_img,phase_img,bic_img,gt_img,msdtgp_img,[50,50,100,100],0,0,windowname)



#挑选效果图
def choice_scene(scene_dict):
    scenes=["000","001","002","003","004","005","006","007","008","009"]
    # scenes=["001","002","003","004","005"]
    for scene in scenes:
        files = scene_dict.get(scene)
        #生成Bicubic的残差结果
        file_name=random.choices(files)[0]
        choice_Windows(scene,file_name)
    # scenes = scene_dict.keys()
    # scene=random.choice(scenes)
    # files=scene_dict[scene]
    # file=random.choice(files)
    # choice_Windows(scene,file)


def product_legend():
    array_a = np.linspace(0,255,300)
    array_b = np.stack([array_a for i in  range(25)])
    array_b = np.expand_dims(array_b,2).astype(np.uint8)
    arra_color = cv2.applyColorMap(array_b.astype(np.uint8), cv2.COLORMAP_JET)
    path=os.path.join(SAVE_Dir,"jetLengend.png")
    cv2.imwrite(path,arra_color)
    # plt.imshow(arra_color[:,:,::-1])
    # plt.show()
    # ccc=0

def reproduct_resisual(ori_file,residual_file):
    ori_image = cv2.imread(ori_file)
    residual_img = cv2.imread(residual_file)
    cv2.imshow("aa",residual_img[:,:,-1])
    cv2.waitKey(0)
    residual_img_add = 0.1 * residual_img + ori_image



def main():
    # #获取文件的目录结构
    # scene_files = getFileDict()
    # # product_residual(scene_files)
    # choice_scene(scene_files)
    # product_legend()
    ori_file="/home/lenovo/Documents/code/10-PhaseVSR_final/10-PhaseVSR/results_compare/Save_Dir/8ea1795e/phasevsr_img_area.png"
    residual_file="/home/lenovo/Documents/code/10-PhaseVSR_final/10-PhaseVSR/results_compare/Save_Dir/8ea1795e/phasevsr_img_resdiual.png"


    reproduct_resisual(ori_file,residual_file)


    print(f"deal ok!")
    pass


if __name__ == '__main__':
    main()