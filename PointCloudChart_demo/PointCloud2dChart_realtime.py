import serial
import json
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.colors import Normalize
from matplotlib import cm

ser = serial.Serial('COM5', 115200, timeout=1)

# 실시간 데이터 시각화를 위한 애니메이션 함수 - 2D 프로젝션으로 변경
def animate_targets_2d(serial_reader):
    """
    실시간으로 들어오는 타겟 데이터를 XY, YZ, XZ 2D 프로젝션으로 애니메이션화
    
    Args:
        serial_reader: 시리얼 데이터를 읽고 JSON 형식으로 반환하는 함수
    """
    fig = plt.figure(figsize=(20, 5))
    
    # 3개의 2D 프로젝션을 위한 서브플롯 생성
    ax_xy = fig.add_subplot(131)  # XY 평면
    ax_yz = fig.add_subplot(132)  # YZ 평면
    ax_xz = fig.add_subplot(133)  # XZ 평면
    
    norm = Normalize(vmin=-5, vmax=5)
    
    # 각 서브플롯에 빈 산점도 생성
    scatter_xy = ax_xy.scatter([], [], s=30, c=[], cmap=cm.magma, marker='o', alpha=0.8, edgecolors='w', norm=norm)
    scatter_yz = ax_yz.scatter([], [], s=30, c=[], cmap=cm.magma, marker='o', alpha=0.8, edgecolors='w', norm=norm)
    scatter_xz = ax_xz.scatter([], [], s=30, c=[], cmap=cm.magma, marker='o', alpha=0.8, edgecolors='w', norm=norm)
    
    # 컬러바 추가
    cbar = plt.colorbar(scatter_xy, ax=[ax_xy, ax_yz, ax_xz], pad=0.01, aspect=30)
    cbar.set_label('Speed (cm/s)', rotation=270, labelpad=15)
    
    plot_num = 0
    
    # 축 레이블
    ax_xy.set_xlabel('X (m)')
    ax_xy.set_ylabel('Y (m)')
    ax_xy.set_title('XY Projection')
    
    ax_yz.set_xlabel('Y (m)')
    ax_yz.set_ylabel('Z (m)')
    ax_yz.set_title('YZ Projection')
    
    ax_xz.set_xlabel('X (m)')
    ax_xz.set_ylabel('Z (m)')
    ax_xz.set_title('XZ Projection')
    
    # 메인 타이틀
    main_title = plt.suptitle(f"2D Projections - #{plot_num}", fontsize=14, fontweight='bold')
    subtitle = fig.text(0.5, 0.01, "Waiting for targets...", fontsize=12, ha='center')
    
    # 축 범위 설정
    for ax in [ax_xy, ax_yz, ax_xz]:
        ax.set_xlim([-3, 3])
        ax.set_ylim([-3, 3])
        
    def update(frame):
        nonlocal plot_num
        # 시리얼에서 새 데이터 읽기
        json_data = serial_reader()
        if not json_data:
            return scatter_xy, scatter_yz, scatter_xz
        
        data = json.loads(json_data) if isinstance(json_data, str) else json_data
        print(data)
        targets = data.get('targets', [])
        timestamp = data.get('timestamp', None)
        
        if not targets:
            subtitle.set_text("No targets detected")
            return scatter_xy, scatter_yz, scatter_xz
        
        # 데이터 추출
        x_points = [target['x_point'] for target in targets]
        y_points = [target['y_point'] for target in targets]
        z_points = [target['z_point'] for target in targets]
        speeds = [target['move_speed'] for target in targets]
        
        subtitle.set_text(f"Target Count: {len(targets)}")
        
        # 산점도 업데이트
        scatter_xy.set_offsets(np.column_stack((x_points, y_points)))
        scatter_yz.set_offsets(np.column_stack((y_points, z_points)))
        scatter_xz.set_offsets(np.column_stack((x_points, z_points)))
        
        # 속도에 따른 색상 매핑
        scatter_xy.set_array(np.array(speeds))
        scatter_yz.set_array(np.array(speeds))
        scatter_xz.set_array(np.array(speeds))
        
        plot_num += 1
        main_title.set_text(f"2D Projections - #{plot_num} ({timestamp}ms)")
        
        # 이미지 저장
        fig.savefig(f"./PointCloud2dChartPNG/2D_Projections_{plot_num}_{timestamp}ms.png", dpi=300, bbox_inches='tight')
        
        return scatter_xy, scatter_yz, scatter_xz
    
    ani = animation.FuncAnimation(fig, update, interval=100, blit=False)
   
    plt.show()

def read_json_from_serial():
    # 시리얼 포트에서 데이터 읽기 및 JSON으로 변환
    try:
        line = ser.readline().decode('utf-8').strip()
        if line.startswith('{') and line.endswith('}'):
            return line
    except:
        pass
    return None

# 애니메이션 실행
animate_targets_2d(read_json_from_serial)