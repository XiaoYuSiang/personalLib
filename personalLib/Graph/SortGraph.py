import sys
from PIL import Image

args = sys.argv[1:]
image_files = args[:-3]
intAx = int(args[-3])
intAy = int(args[-2])
output_file = args[-1]

def combine_images():
    grid_size=(intAx, intAy) 
    # 打开所有图片
    images = [Image.open(file) for file in image_files]
    
    # 假设所有图片的尺寸相同，获取单张图片的宽和高
    img_width, img_height = images[0].size
    
    # 创建新图片，尺寸为单张图片宽和高的乘积
    combined_image = Image.new('RGB', (img_width * grid_size[0], img_height * grid_size[1]))
    
    # 将图片粘贴到新图片上
    for index, image in enumerate(images):
        x = (index % grid_size[0]) * img_width
        y = (index // grid_size[0]) * img_height
        combined_image.paste(image, (x, y))
    
    # 保存合成后的图片
    combined_image.save(output_file)


# 调用函数进行合成
combine_images()
