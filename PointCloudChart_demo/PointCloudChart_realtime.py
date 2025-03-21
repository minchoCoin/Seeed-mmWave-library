import serial
import json
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.animation as animation
from matplotlib.colors import Normalize
from matplotlib import cm

ser = serial.Serial('COM5', 115200, timeout=1)

# 실시간 데이터 시각화를 위한 애니메이션 함수
def animate_targets_3d(serial_reader):
    """
    실시간으로 들어오는 타겟 데이터를 애니메이션으로 표시
    
    Args:
        serial_reader: 시리얼 데이터를 읽고 JSON 형식으로 반환하는 함수
    """
    fig = plt.figure(figsize=(10, 8))
    ax = fig.add_subplot(111, projection='3d')
    norm = Normalize(vmin=-5, vmax=5)
    # 초기 산점도 (빈 데이터로 시작)
    scatter = ax.scatter([], [], [], s=30, c=[], cmap=cm.magma, marker='o', alpha=0.8, edgecolors='w',norm=norm)
    
    # 컬러바 추가
    cbar = plt.colorbar(scatter, ax=ax, pad=0.1)
    cbar.set_label('Speed (cm/s)', rotation=270, labelpad=15)
    

    plot_num = 0
    # 축 레이블
    ax.set_xlabel('X (m)')
    ax.set_ylabel('Y (m)')
    ax.set_zlabel('Z (m)')
    title_obj = ax.set_title(f"3D Point Cloud - #{plot_num}",fontsize=12, fontweight='bold', pad=20)
    subtitle = plt.suptitle("Waiting for targets...", fontsize=12, y=0.98)
    # 축 범위 설정 (필요에 따라 조정)
    ax.set_xlim([-1.5, 1.5])
    ax.set_ylim([-1.5, 1.5])
    ax.set_zlim([-1, 2])
    
    # 텍스트 객체를 저장할 리스트
    texts = []
    
    def update(frame):
        nonlocal plot_num
        # 시리얼에서 새 데이터 읽기
        json_data = serial_reader()  
        if not json_data:
            return scatter,
        
        data = json.loads(json_data) if isinstance(json_data, str) else json_data
        print(data)
        targets = data.get('targets', [])
        timestamp = data.get('timestamp', None)
        if not targets:
            subtitle.set_text("No targets detected")
            return scatter,
        
        # 데이터 추출
        x_points = [target['x_point'] for target in targets]
        y_points = [target['y_point'] for target in targets]
        z_points = [target['z_point'] for target in targets]
        speeds = [target['move_speed'] for target in targets]
        target_ids = [target['target_id'] for target in targets]
        subtitle.set_text(f"Target Count: {len(targets)}")
        # 이전 텍스트 제거
       # for txt in texts:
       #     txt.remove()
       # texts.clear()
        
        # 산점도 업데이트
        scatter._offsets3d = (x_points, y_points, z_points)
        
        # 속도에 따른 색상 매핑
        #norm = Normalize(vmin=min(speeds), vmax=max(speeds))
        scatter.set_array(np.array(speeds))
        
        plot_num+=1
        title_obj.set_text(f"3D Point Cloud - #{plot_num} ({timestamp}ms)")
        
        fig.savefig(f"./PointCloudChartPNG/3D Point Cloud - {plot_num} ({timestamp}ms).png",dpi=300,bbox_inches='tight')
        
        # ID 라벨 업데이트
       # for i, txt in enumerate(target_ids):
        #    t = ax.text(x_points[i], y_points[i], z_points[i], f'ID:{txt}', size=9)
        #    texts.append(t)
        
        return scatter,
    
    ani = animation.FuncAnimation(fig, update, interval=100, blit=False)
    plt.tight_layout()
    plt.show()

def read_json_from_serial():
    # 시리얼 포트 설정
    
    
    # 데이터 읽기 및 JSON으로 변환
    try:
        line = ser.readline().decode('utf-8').strip()
        if line.startswith('{') and line.endswith('}'):
            return line
    except:
        pass
    return None

# 애니메이션 실행
animate_targets_3d(read_json_from_serial)