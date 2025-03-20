import serial
import json
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.animation as animation
from matplotlib.colors import Normalize
from matplotlib import cm

def visualize_targets_3d(json_data,n):
    if isinstance(json_data, str):
        data = json.loads(json_data)
    else:
        data = json_data

    targets=data.get('targets',[])

    if not targets:
        print('no targets...!')
        return
    
    x_points = [target['x_point'] for target in targets]
    y_points = [target['y_point'] for target in targets]
    z_points = [target['z_point'] for target in targets]
    speeds = [target['move_speed'] for target in targets]
    target_ids = [target['target_id'] for target in targets]

    fig = plt.figure(figsize=(10, 8))
    ax = fig.add_subplot(111, projection='3d')

    norm = Normalize(vmin=min(speeds), vmax=max(speeds))
    cmap = cm.plasma
    colors = cmap(norm(speeds))

    scatter = ax.scatter(x_points, y_points, z_points, c=speeds, cmap=cmap, 
                         marker='o', alpha=0.8, edgecolors='w')
    
    cbar = plt.colorbar(scatter, ax=ax, pad=0.1)
    cbar.set_label('Speed (cm/s)', rotation=270, labelpad=15)


    # 축 범위 설정 (필요에 따라 조정)
    ax.set_xlim([-1.5, 1.5])
    ax.set_ylim([-1.5, 1.5])
    ax.set_zlim([-1, 2])
    
    ax.set_xlabel('X (m)')
    ax.set_ylabel('Y (m)')
    ax.set_zlabel('Z (m)')
    ax.set_title(f'Point Cloud with Speed information - {n}')

     # grid
    ax.grid(True)
    
    # save
    plt.tight_layout()
    #plt.show()
    plt.savefig(f'./PointCloudChartPNG/PointCloud {n}.png')

    
def read_json_from_serial(port, baud_rate=115200):
    i=0
    with serial.Serial(port, baud_rate, timeout=1) as ser:
        while True:
            line = ser.readline().decode('utf-8').strip()
            if line.startswith('{') and line.endswith('}'):
                try:
                    data = json.loads(line)
                    print(data)  # Process your JSON data here
                    visualize_targets_3d(data,i)
                    i=i+1
                except json.JSONDecodeError:
                    print("Invalid JSON received")


read_json_from_serial('COM5', 115200)