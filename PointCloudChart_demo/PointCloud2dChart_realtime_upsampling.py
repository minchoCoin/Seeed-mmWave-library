import serial
import json
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.colors import Normalize
from matplotlib import cm
from sklearn.neighbors import NearestNeighbors
from scipy.spatial import Voronoi

# 시리얼 포트 설정
ser = serial.Serial('COM5', 115200, timeout=1)

# 업샘플링 메서드 1: 보간법 업샘플링
def interpolation_upsampling(points, speeds, factor=2, k=3):
    """
    기존 포인트 주변에 새로운 포인트를 추가하여 업샘플링합니다.
    
    Parameters:
    points (numpy.ndarray): 원본 포인트 클라우드 (N x 3)
    speeds (numpy.ndarray): 각 포인트의 속도 값 (N)
    factor (int): 업샘플링 비율
    k (int): 고려할 이웃 포인트 수
    
    Returns:
    tuple: (업샘플링된 포인트 클라우드, 업샘플링된 속도 값)
    """
    if len(points) < k:
        return points, speeds  # 포인트가 충분하지 않으면 원본 반환
    
    # KNN을 사용하여 각 포인트의 k개 이웃 찾기
    nbrs = NearestNeighbors(n_neighbors=min(k, len(points)), algorithm='auto').fit(points)
    distances, indices = nbrs.kneighbors(points)
    
    new_points = []
    new_speeds = []
    
    # 원본 포인트와 속도 추가
    for i in range(len(points)):
        new_points.append(points[i])
        new_speeds.append(speeds[i])
    
    # 새 포인트 생성
    for i, neighbors in enumerate(indices):
        for _ in range(factor - 1):
            # 랜덤한 두 이웃 선택 (충분한 이웃이 있는 경우)
            valid_neighbors = neighbors[neighbors != i]  # 자기 자신 제외
            if len(valid_neighbors) >= 2:
                idx1, idx2 = np.random.choice(valid_neighbors, 2, replace=False)
            elif len(valid_neighbors) == 1:
                idx1 = valid_neighbors[0]
                idx2 = i
            else:
                continue
            
            # 이웃 사이의 중간 지점 계산(약간의 랜덤성 추가)
            mid_point = (points[idx1] + points[idx2]) / 2
            noise = np.random.normal(0, 0.02, 3)  # 약간의 노이즈 추가
            new_points.append(mid_point + noise)
            
            # 속도값도 보간
            mid_speed = (speeds[idx1] + speeds[idx2]) / 2
            speed_noise = np.random.normal(0, 0.1)  # 속도에 약간의 노이즈
            new_speeds.append(mid_speed + speed_noise)
    
    return np.array(new_points), np.array(new_speeds)

# 업샘플링 메서드 2: MLS 기반 업샘플링 (단순화된 버전)
def mls_upsampling(points, speeds, factor=2, search_radius=0.1):
    """
    MLS 기반 접근법으로 포인트 클라우드 업샘플링
    
    Parameters:
    points (numpy.ndarray): 원본 포인트 클라우드 (N x 3)
    speeds (numpy.ndarray): 각 포인트의 속도 값
    factor (int): 업샘플링 비율
    search_radius (float): 탐색 반경
    
    Returns:
    tuple: (업샘플링된 포인트 클라우드, 업샘플링된 속도 값)
    """
    if len(points) < 3:
        return points, speeds  # 포인트가 충분하지 않으면 원본 반환
    
    new_points = []
    new_speeds = []
    
    # 원본 포인트 및 속도 추가
    for i in range(len(points)):
        new_points.append(points[i])
        new_speeds.append(speeds[i])
    
    # 각 포인트에 대해
    for i, point in enumerate(points):
        # 인접 포인트 찾기
        dists = np.sqrt(np.sum((points - point)**2, axis=1))
        neighbors = np.where(dists < search_radius)[0]
        neighbors = neighbors[neighbors != i]  # 자기 자신 제외
        
        if len(neighbors) < 2:
            continue
        
        # 근사적인 법선 벡터 계산 (PCA 사용)
        neighbor_points = points[neighbors]
        centered = neighbor_points - np.mean(neighbor_points, axis=0)
        
        # 공분산 행렬 계산
        cov = np.cov(centered, rowvar=False)
        
        try:
            # 가장 작은 고유값에 해당하는 고유벡터가 법선 벡터
            eigenvalues, eigenvectors = np.linalg.eigh(cov)
            normal = eigenvectors[:, 0]  # 가장 작은 고유값에 해당하는 고유벡터
            
            # 법선 벡터와 직교하는 벡터 생성
            if abs(normal[0]) > abs(normal[1]):
                v1 = np.array([normal[2], 0, -normal[0]])
            else:
                v1 = np.array([0, normal[2], -normal[1]])
            
            v1 = v1 / (np.linalg.norm(v1) + 1e-10)
            v2 = np.cross(normal, v1)
            
            # 새 포인트 생성
            for _ in range(factor - 1):
                theta = np.random.uniform(0, 2 * np.pi)
                r = np.random.uniform(0, search_radius * 0.5)
                
                # 지역 평면 위에 새 포인트 생성
                offset = r * (np.cos(theta) * v1 + np.sin(theta) * v2)
                new_point = point + offset
                new_points.append(new_point)
                
                # 속도는 거리에 따른 가중 평균으로 계산
                weights = 1.0 / (dists[neighbors] + 1e-10)
                weights = weights / np.sum(weights)
                avg_speed = np.sum(weights * speeds[neighbors])
                new_speeds.append(avg_speed)
                
        except np.linalg.LinAlgError:
            # SVD 계산 실패 시 단순히 이웃 사이의 중간점 사용
            for _ in range(factor - 1):
                idx1, idx2 = np.random.choice(neighbors, 2, replace=len(neighbors) < 2)
                mid_point = (points[idx1] + points[idx2]) / 2
                noise = np.random.normal(0, 0.01, 3)
                new_points.append(mid_point + noise)
                new_speeds.append((speeds[idx1] + speeds[idx2]) / 2)
    
    return np.array(new_points), np.array(new_speeds)

# 업샘플링 메서드 3: 보로노이 기반 업샘플링 (단순화된 버전)
def voronoi_upsampling(points, speeds, bounds=None, max_points_to_add=100):
    """
    보로노이 다이어그램을 사용한 포인트 클라우드 업샘플링
    
    Parameters:
    points (numpy.ndarray): 원본 포인트 클라우드 (N x 3)
    speeds (numpy.ndarray): 각 포인트의 속도 값
    bounds (tuple): 바운딩 박스 (min_x, max_x, min_y, max_y, min_z, max_z)
    max_points_to_add (int): 최대 추가 포인트 수
    
    Returns:
    tuple: (업샘플링된 포인트 클라우드, 업샘플링된 속도 값)
    """
    if len(points) < 4:  # 보로노이에는 최소 4개의 포인트가 필요
        return points, speeds
    
    # 바운딩 박스 계산
    if bounds is None:
        min_bounds = np.min(points, axis=0) - 0.1
        max_bounds = np.max(points, axis=0) + 0.1
        bounds = (min_bounds[0], max_bounds[0], 
                 min_bounds[1], max_bounds[1], 
                 min_bounds[2], max_bounds[2])
    
    # 2D 보로노이 계산 (XY 평면으로 투영)
    try:
        points_2d = points[:, :2]
        vor = Voronoi(points_2d)
        
        # 보로노이 정점 필터링
        valid_vertices = []
        for vertex in vor.vertices:
            if (bounds[0] <= vertex[0] <= bounds[1] and
                bounds[2] <= vertex[1] <= bounds[3]):
                
                # 가장 가까운 k개 포인트 찾기
                dists = np.sqrt(np.sum((points_2d - vertex) ** 2, axis=1))
                k = min(3, len(points))
                nearest_indices = np.argsort(dists)[:k]
                
                # Z 좌표 계산 (가중 평균)
                weights = 1.0 / (dists[nearest_indices] + 1e-10)
                weights = weights / np.sum(weights)
                z_coord = np.sum(weights * points[nearest_indices, 2])
                
                # Z 좌표가 바운드 내에 있는지 확인
                if bounds[4] <= z_coord <= bounds[5]:
                    valid_vertices.append((vertex[0], vertex[1], z_coord))
                    
        # 속도 값 계산
        new_speeds = []
        for vertex in valid_vertices:
            dists = np.sqrt(np.sum((points[:, :2] - vertex[:2]) ** 2, axis=1))
            nearest_indices = np.argsort(dists)[:3]
            weights = 1.0 / (dists[nearest_indices] + 1e-10)
            weights = weights / np.sum(weights)
            avg_speed = np.sum(weights * speeds[nearest_indices])
            new_speeds.append(avg_speed)
        
        # 최대 포인트 수 제한
        if len(valid_vertices) > max_points_to_add:
            indices = np.random.choice(len(valid_vertices), max_points_to_add, replace=False)
            valid_vertices = [valid_vertices[i] for i in indices]
            new_speeds = [new_speeds[i] for i in indices]
        
        # 원본 포인트와 새 포인트 합치기
        upsampled_points = np.vstack([points, valid_vertices])
        upsampled_speeds = np.concatenate([speeds, new_speeds])
        
        return upsampled_points, upsampled_speeds
        
    except (ValueError, np.linalg.LinAlgError):
        # 보로노이 계산 실패 시 원본 반환
        return points, speeds

# 실시간 데이터 시각화를 위한 애니메이션 함수 - 업샘플링 적용
def animate_targets_2d(serial_reader, upsampling_method='interpolation'):
    """
    실시간으로 들어오는 타겟 데이터를 업샘플링하고 XY, YZ, XZ 2D 프로젝션으로 애니메이션화
    
    Args:
        serial_reader: 시리얼 데이터를 읽고 JSON 형식으로 반환하는 함수
        upsampling_method: 업샘플링 방법 ('interpolation', 'mls', 'voronoi')
    """
    fig = plt.figure(figsize=(20, 5))
    
    # 3개의 2D 프로젝션을 위한 서브플롯 생성
    ax_xy = fig.add_subplot(131)  # XY 평면
    ax_yz = fig.add_subplot(132)  # YZ 평면
    ax_xz = fig.add_subplot(133)  # XZ 평면
    
    norm = Normalize(vmin=-5, vmax=5)
    
    # 원본 데이터용 산점도
    scatter_xy_orig = ax_xy.scatter([], [], s=30, c=[], cmap=cm.magma, marker='o', alpha=0.8, edgecolors='w', norm=norm, label='Original')
    scatter_yz_orig = ax_yz.scatter([], [], s=30, c=[], cmap=cm.magma, marker='o', alpha=0.8, edgecolors='w', norm=norm, label='Original')
    scatter_xz_orig = ax_xz.scatter([], [], s=30, c=[], cmap=cm.magma, marker='o', alpha=0.8, edgecolors='w', norm=norm, label='Original')
    
    # 업샘플링 데이터용 산점도
    scatter_xy_up = ax_xy.scatter([], [], s=15, c=[], cmap=cm.magma, marker='x', alpha=0.5, edgecolors='w', norm=norm, label='Upsampled')
    scatter_yz_up = ax_yz.scatter([], [], s=15, c=[], cmap=cm.magma, marker='x', alpha=0.5, edgecolors='w', norm=norm, label='Upsampled')
    scatter_xz_up = ax_xz.scatter([], [], s=15, c=[], cmap=cm.magma, marker='x', alpha=0.5, edgecolors='w', norm=norm, label='Upsampled')
    
    # 각 데이터셋에 대한 별도의 컬러바 추가
    # 원본 데이터용 컬러바
    cbar_orig = plt.colorbar(scatter_xy_orig, ax=[ax_xy, ax_yz, ax_xz], pad=0.01, aspect=30)
    cbar_orig.set_label('Original Speed (cm/s)', rotation=270, labelpad=15)
    
    # 업샘플링 데이터용 컬러바
    cbar_up = plt.colorbar(scatter_xy_up, ax=[ax_xy, ax_yz, ax_xz], pad=0.01, aspect=30)
    cbar_up.set_label('Upsampled Speed (cm/s)', rotation=270, labelpad=15)
    
    plot_num = 0
    
    # 축 레이블
    ax_xy.set_xlabel('X (m)')
    ax_xy.set_ylabel('Y (m)')
    ax_xy.set_title('XY Projection')
    ax_xy.legend()
    
    ax_yz.set_xlabel('Y (m)')
    ax_yz.set_ylabel('Z (m)')
    ax_yz.set_title('YZ Projection')
    ax_yz.legend()
    
    ax_xz.set_xlabel('X (m)')
    ax_xz.set_ylabel('Z (m)')
    ax_xz.set_title('XZ Projection')
    ax_xz.legend()
    
    # 메인 타이틀
    
    main_title = plt.suptitle(f"2D Projections with {upsampling_method} Upsampling - #{plot_num}", 
                             fontsize=14, fontweight='bold')
    subtitle = fig.text(0.5, 0.01, "Waiting for targets...", fontsize=12, ha='center')
    
    # 축 범위 설정
    ax_xy.set_xlim([-0.5, 0.5])
    ax_xy.set_ylim([0, 2])

    ax_yz.set_xlim([0, 2])
    ax_yz.set_ylim([-0.5, 1])

    ax_xz.set_xlim([-0.7, 0.7])
    ax_xz.set_ylim([-1, 1])

    def update(frame):
        nonlocal plot_num
        # 시리얼에서 새 데이터 읽기
        json_data = serial_reader()
        if not json_data:
            return scatter_xy_orig, scatter_yz_orig, scatter_xz_orig, scatter_xy_up, scatter_yz_up, scatter_xz_up
        
        try:
            data = json.loads(json_data) if isinstance(json_data, str) else json_data
        except json.JSONDecodeError:
            print("Invalid JSON received. continued...")
            return scatter_xy_orig, scatter_yz_orig, scatter_xz_orig, scatter_xy_up, scatter_yz_up, scatter_xz_up
        
        targets = data.get('targets', [])
        timestamp = data.get('timestamp', None)
        
        if not targets:
            subtitle.set_text("No targets detected")
            return scatter_xy_orig, scatter_yz_orig, scatter_xz_orig, scatter_xy_up, scatter_yz_up, scatter_xz_up
        
        # 데이터 추출 - 원본 포인트
        x_points = np.array([target['x_point'] for target in targets])
        y_points = np.array([target['y_point'] for target in targets])
        z_points = np.array([target['z_point'] for target in targets])
        speeds = np.array([target['move_speed'] for target in targets])
        
        # 원본 포인트 배열 구성
        orig_points = np.column_stack((x_points, y_points, z_points))
        
        # 선택한 방식으로 업샘플링 수행
        if upsampling_method == 'interpolation':
            upsampled_points, upsampled_speeds = interpolation_upsampling(orig_points, speeds, factor=3, k=3)
        elif upsampling_method == 'mls':
            upsampled_points, upsampled_speeds = mls_upsampling(orig_points, speeds, factor=3, search_radius=0.15)
        elif upsampling_method == 'voronoi':
            upsampled_points, upsampled_speeds = voronoi_upsampling(orig_points, speeds, max_points_to_add=len(orig_points)*2)
        else:
            # 기본값은 보간법
            upsampled_points, upsampled_speeds = interpolation_upsampling(orig_points, speeds, factor=3, k=3)
        n_orig = len(orig_points)
        # 새로 생성된 포인트만 선택
        upsampled_points = upsampled_points[n_orig:]
        upsampled_speeds = upsampled_speeds[n_orig:]
        
        # 업샘플링된 데이터가 비어있지 않은지 확인
        if len(upsampled_points) > 0:
            up_x = upsampled_points[:, 0]
            up_y = upsampled_points[:, 1]
            up_z = upsampled_points[:, 2]
        else:
            # 업샘플링된 포인트가 없는 경우 빈 배열 생성
            up_x = np.array([])
            up_y = np.array([])
            up_z = np.array([])
        
        # 상태 텍스트 업데이트
        subtitle.set_text(f"Target Count: {len(targets)} (Original) + {len(upsampled_points)} (New Points)")
        
        
        
        # 원본 산점도 업데이트
        scatter_xy_orig.set_offsets(np.column_stack((x_points, y_points)))
        scatter_xy_orig.set_array(np.array(speeds))
        
        scatter_yz_orig.set_offsets(np.column_stack((y_points, z_points)))
        scatter_yz_orig.set_array(np.array(speeds))
        
        scatter_xz_orig.set_offsets(np.column_stack((x_points, z_points)))
        scatter_xz_orig.set_array(np.array(speeds))
        
        # 업샘플링된 산점도 업데이트
        scatter_xy_up.set_offsets(np.column_stack((up_x, up_y)))
        scatter_xy_up.set_array(np.array(upsampled_speeds))
        
        scatter_yz_up.set_offsets(np.column_stack((up_y, up_z)))
        scatter_yz_up.set_array(np.array(upsampled_speeds))
        
        scatter_xz_up.set_offsets(np.column_stack((up_x, up_z)))
        scatter_xz_up.set_array(np.array(upsampled_speeds))
        
        plot_num += 1
        main_title.set_text(f"2D Projections with {upsampling_method} Upsampling - #{plot_num} ({timestamp}ms)")
        
        # 이미지 저장 (필요시 주석 해제)
        #fig.savefig(f"./PointCloud2dChartPNG/Upsampled_{upsampling_method}_{plot_num}_{timestamp}ms.png", dpi=300, bbox_inches='tight')
        
        return scatter_xy_orig, scatter_yz_orig, scatter_xz_orig, scatter_xy_up, scatter_yz_up, scatter_xz_up
    
    ani = animation.FuncAnimation(fig, update, interval=100, blit=False, save_count=100)
   
    
    plt.show()

def read_json_from_serial():
    # 시리얼 포트에서 데이터 읽기 및 JSON으로 변환
    try:
        line = ser.readline().decode('utf-8').strip()
        if line.startswith('{') and line.endswith('}'):
            return line
        else:
            print(line)
    except:
        pass
    return None

# 업샘플링 방법 선택 및 애니메이션 실행
# 'interpolation', 'mls', 'voronoi' 중 선택
selected_method = 'voronoi'  # 기본값: MLS 방법 (가장 부드러운 결과)

animate_targets_2d(read_json_from_serial, upsampling_method=selected_method)