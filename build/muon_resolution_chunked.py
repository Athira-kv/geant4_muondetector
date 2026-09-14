#!/usr/bin/env python3
"""
Muon Tomography: Resolution Study
Optimized for large files with chunked processing
"""

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.ndimage import gaussian_filter, sobel
import sys

# ============================================================
# CONFIGURATION
# ============================================================

X_RANGE = (-15, 15)
Y_RANGE = (-15, 15)
Z_SLICE = (-15, 15)
CHUNK_SIZE = 500000
PIXEL_SIZES = [0.25, 0.5, 0.75, 1.0, 1.5, 2.0]

CONFIG1 = {
    'name': 'Configuration 1',
    'cylinders': [
        {'mat': 'W',  'Z': 74, 'x': -5, 'y':  5, 'r': 2.0},
        {'mat': 'Pb', 'Z': 82, 'x':  5, 'y':  5, 'r': 2.0},
        {'mat': 'Fe', 'Z': 26, 'x': -5, 'y': -5, 'r': 2.0},
        {'mat': 'Al', 'Z': 13, 'x':  5, 'y': -5, 'r': 2.0},
    ]
}

CONFIG2 = {
    'name': 'Configuration 2',
    'cylinders': [
        {'mat': 'U',  'Z': 92, 'x':  0, 'y':  6, 'r': 1.5},
        {'mat': 'W',  'Z': 74, 'x': -6, 'y':  0, 'r': 2.0},
        {'mat': 'Pb', 'Z': 82, 'x':  6, 'y':  0, 'r': 2.0},
        {'mat': 'Fe', 'Z': 26, 'x': -4, 'y': -5, 'r': 1.5},
        {'mat': 'Al', 'Z': 13, 'x':  4, 'y': -5, 'r': 2.5},
        {'mat': 'C',  'Z':  6, 'x':  0, 'y':  0, 'r': 1.0},
    ]
}

# ============================================================
# TRACK FITTING
# ============================================================

def fit_line_3d(points):
    if len(points) < 2:
        return None, None
    centroid = np.mean(points, axis=0)
    _, _, Vt = np.linalg.svd(points - centroid)
    direction = Vt[0]
    if direction[2] > 0:
        direction = -direction
    return centroid, direction

def compute_poca(p1, d1, p2, d2):
    d1, d2 = d1/np.linalg.norm(d1), d2/np.linalg.norm(d2)
    if np.linalg.norm(np.cross(d1, d2)) < 1e-10:
        return None, None
    w0 = p1 - p2
    a, b, c = np.dot(d1,d1), np.dot(d1,d2), np.dot(d2,d2)
    d, e = np.dot(d1,w0), np.dot(d2,w0)
    denom = a*c - b*b
    if abs(denom) < 1e-10:
        return None, None
    t, s = (b*e-c*d)/denom, (a*e-b*d)/denom
    poca = (p1 + t*d1 + p2 + s*d2) / 2
    theta = np.arccos(np.clip(abs(np.dot(d1, d2)), 0, 1))
    return poca, theta

def process_event_group(group):
    muon_hits = group[group['particle'].isin(['mu-', 'mu+'])]
    
    dc1 = muon_hits[muon_hits['detector_name'] == 'DriftChamber1'][['x_cm','y_cm','z_cm']].values
    dc2 = muon_hits[muon_hits['detector_name'] == 'DriftChamber2'][['x_cm','y_cm','z_cm']].values
    upper = np.vstack([dc1, dc2]) if len(dc1) and len(dc2) else (dc1 if len(dc1) >= 2 else dc2)
    
    if len(upper) < 2:
        return None
    
    dc3 = muon_hits[muon_hits['detector_name'] == 'DriftChamber3'][['x_cm','y_cm','z_cm']].values
    dc4 = muon_hits[muon_hits['detector_name'] == 'DriftChamber4'][['x_cm','y_cm','z_cm']].values
    lower = np.vstack([dc3, dc4]) if len(dc3) and len(dc4) else (dc3 if len(dc3) >= 2 else dc4)
    
    if len(lower) < 2:
        return None
    
    p_in, d_in = fit_line_3d(upper)
    p_out, d_out = fit_line_3d(lower)
    
    if p_in is None or p_out is None:
        return None
    
    poca, theta = compute_poca(p_in, d_in, p_out, d_out)
    
    if poca is None:
        return None
    if not (Z_SLICE[0] <= poca[2] <= Z_SLICE[1]):
        return None
    
    return (poca[0], poca[1], theta)

# ============================================================
# CHUNKED PROCESSING - COLLECT SCATTER DATA
# ============================================================

def process_file_to_scatter_data(filename):
    """Extract all valid PoCA points and save to intermediate file"""
    
    scatter_data = []
    total_events = 0
    valid_events = 0
    chunk_num = 0
    leftover = None
    
    print(f"Processing {filename} in chunks of {CHUNK_SIZE} rows...")
    
    for chunk in pd.read_csv(filename, chunksize=CHUNK_SIZE):
        chunk_num += 1
        
        if leftover is not None:
            chunk = pd.concat([leftover, chunk], ignore_index=True)
        
        last_event = chunk['event_id'].iloc[-1]
        complete = chunk[chunk['event_id'] != last_event]
        leftover = chunk[chunk['event_id'] == last_event]
        
        events = complete.groupby('event_id')
        
        for evt_id, group in events:
            total_events += 1
            result = process_event_group(group)
            
            if result is not None:
                scatter_data.append(result)
                valid_events += 1
        
        print(f"  Chunk {chunk_num}: {total_events} events, {valid_events} valid")
    
    # Process leftover
    if leftover is not None and len(leftover) > 0:
        for evt_id, group in leftover.groupby('event_id'):
            result = process_event_group(group)
            if result is not None:
                scatter_data.append(result)
                valid_events += 1
    
    print(f"\nTotal: {total_events} events, {valid_events} valid ({100*valid_events/total_events:.1f}%)")
    
    scatter_df = pd.DataFrame(scatter_data, columns=['x', 'y', 'theta'])
    return scatter_df

# ============================================================
# RECONSTRUCTION AT DIFFERENT RESOLUTIONS
# ============================================================

def reconstruct_at_resolution(scatter_df, pixel_size):
    n_pixels = int((X_RANGE[1] - X_RANGE[0]) / pixel_size)
    x_bins = np.linspace(X_RANGE[0], X_RANGE[1], n_pixels + 1)
    y_bins = np.linspace(Y_RANGE[0], Y_RANGE[1], n_pixels + 1)
    
    x_idx = np.digitize(scatter_df['x'].values, x_bins) - 1
    y_idx = np.digitize(scatter_df['y'].values, y_bins) - 1
    
    count = np.zeros((n_pixels, n_pixels))
    sum_theta2 = np.zeros((n_pixels, n_pixels))
    theta = scatter_df['theta'].values
    
    for i in range(len(scatter_df)):
        xi, yi = x_idx[i], y_idx[i]
        if 0 <= xi < n_pixels and 0 <= yi < n_pixels:
            count[xi, yi] += 1
            sum_theta2[xi, yi] += theta[i]**2
    
    with np.errstate(divide='ignore', invalid='ignore'):
        rms = np.sqrt(np.where(count > 0, sum_theta2 / count, 0))
    
    return rms.T, count.T

# ============================================================
# RESOLUTION METRICS
# ============================================================

def compute_cnr(rms_img, config, pixel_size):
    n_pixels = int((X_RANGE[1] - X_RANGE[0]) / pixel_size)
    x_centers = np.linspace(X_RANGE[0] + pixel_size/2, X_RANGE[1] - pixel_size/2, n_pixels)
    y_centers = np.linspace(Y_RANGE[0] + pixel_size/2, Y_RANGE[1] - pixel_size/2, n_pixels)
    xx, yy = np.meshgrid(x_centers, y_centers)
    
    bg_mask = np.ones_like(rms_img, dtype=bool)
    for cyl in config['cylinders']:
        dist = np.sqrt((xx - cyl['x'])**2 + (yy - cyl['y'])**2)
        bg_mask &= (dist > cyl['r'] + 2)
    
    bg_values = rms_img[bg_mask & (rms_img > 0)]
    if len(bg_values) < 10:
        return {}
    bg_mean, bg_std = np.mean(bg_values), np.std(bg_values)
    
    cnr_results = {}
    for cyl in config['cylinders']:
        dist = np.sqrt((xx - cyl['x'])**2 + (yy - cyl['y'])**2)
        cyl_mask = dist <= cyl['r']
        cyl_values = rms_img[cyl_mask & (rms_img > 0)]
        
        if len(cyl_values) > 3:
            cyl_mean = np.mean(cyl_values)
            cnr = abs(cyl_mean - bg_mean) / (bg_std + 1e-10)
            cnr_results[cyl['mat']] = {'CNR': cnr, 'signal': cyl_mean, 'n_pixels': len(cyl_values)}
    
    return cnr_results

def compute_edge_sharpness(rms_img, config, pixel_size):
    gradient = np.sqrt(sobel(rms_img, axis=0)**2 + sobel(rms_img, axis=1)**2)
    
    n_pixels = int((X_RANGE[1] - X_RANGE[0]) / pixel_size)
    x_centers = np.linspace(X_RANGE[0] + pixel_size/2, X_RANGE[1] - pixel_size/2, n_pixels)
    y_centers = np.linspace(Y_RANGE[0] + pixel_size/2, Y_RANGE[1] - pixel_size/2, n_pixels)
    xx, yy = np.meshgrid(x_centers, y_centers)
    
    sharpness = {}
    for cyl in config['cylinders']:
        dist = np.sqrt((xx - cyl['x'])**2 + (yy - cyl['y'])**2)
        edge_mask = (dist > cyl['r'] - 1) & (dist < cyl['r'] + 1)
        edge_values = gradient[edge_mask]
        if len(edge_values) > 0:
            sharpness[cyl['mat']] = np.mean(edge_values)
    
    return sharpness

# ============================================================
# PLOTTING
# ============================================================

def plot_resolution_comparison(scatter_df, config, output_prefix):
    fig, axes = plt.subplots(2, 3, figsize=(14, 9))
    fig.suptitle(f"{config['name']}: Resolution Study", fontsize=14, fontweight='bold')
    
    extent = [X_RANGE[0], X_RANGE[1], Y_RANGE[0], Y_RANGE[1]]
    
    for ax, pix in zip(axes.flat, PIXEL_SIZES):
        rms, count = reconstruct_at_resolution(scatter_df, pix)
        rms_smooth = gaussian_filter(rms, sigma=0.5)
        
        vmin, vmax = np.percentile(rms_smooth[rms_smooth > 0], [5, 95]) if np.any(rms_smooth > 0) else (0, 1)
        ax.imshow(rms_smooth, extent=extent, origin='lower', cmap='hot', vmin=vmin, vmax=vmax)
        
        for cyl in config['cylinders']:
            circle = plt.Circle((cyl['x'], cyl['y']), cyl['r'], fill=False, color='white', linestyle='--', linewidth=1)
            ax.add_patch(circle)
        
        ax.set_xlabel('X (cm)')
        ax.set_ylabel('Y (cm)')
        ax.set_title(f'Pixel size = {pix} cm')
    
    plt.tight_layout()
    plt.savefig(f'{output_prefix}_resolution_images.png', dpi=150)
    plt.show()

def plot_resolution_metrics(scatter_df, config, output_prefix):
    all_cnr = {cyl['mat']: [] for cyl in config['cylinders']}
    all_sharpness = {cyl['mat']: [] for cyl in config['cylinders']}
    mean_cnr = []
    mean_sharpness = []
    
    for pix in PIXEL_SIZES:
        rms, _ = reconstruct_at_resolution(scatter_df, pix)
        rms_smooth = gaussian_filter(rms, sigma=0.5)
        
        cnr = compute_cnr(rms_smooth, config, pix)
        sharp = compute_edge_sharpness(rms_smooth, config, pix)
        
        cnr_vals = []
        sharp_vals = []
        for cyl in config['cylinders']:
            mat = cyl['mat']
            if mat in cnr:
                all_cnr[mat].append(cnr[mat]['CNR'])
                cnr_vals.append(cnr[mat]['CNR'])
            else:
                all_cnr[mat].append(np.nan)
            if mat in sharp:
                all_sharpness[mat].append(sharp[mat])
                sharp_vals.append(sharp[mat])
            else:
                all_sharpness[mat].append(np.nan)
        
        mean_cnr.append(np.nanmean(cnr_vals))
        mean_sharpness.append(np.nanmean(sharp_vals))
    
    fig, axes = plt.subplots(1, 3, figsize=(14, 4))
    fig.suptitle(f"{config['name']}: Resolution Metrics", fontsize=14, fontweight='bold')
    
    ax = axes[0]
    for mat, cnr_list in all_cnr.items():
        ax.plot(PIXEL_SIZES, cnr_list, 'o-', label=mat)
    ax.set_xlabel('Pixel Size (cm)')
    ax.set_ylabel('CNR')
    ax.set_title('Contrast-to-Noise Ratio')
    ax.legend()
    ax.grid(True, alpha=0.3)
    
    ax = axes[1]
    for mat, sharp_list in all_sharpness.items():
        ax.plot(PIXEL_SIZES, sharp_list, 'o-', label=mat)
    ax.set_xlabel('Pixel Size (cm)')
    ax.set_ylabel('Edge Gradient')
    ax.set_title('Edge Sharpness')
    ax.legend()
    ax.grid(True, alpha=0.3)
    
    ax = axes[2]
    ax.plot(PIXEL_SIZES, mean_cnr, 'bo-', label='Mean CNR')
    ax.set_xlabel('Pixel Size (cm)')
    ax.set_ylabel('Mean CNR', color='b')
    ax.tick_params(axis='y', labelcolor='b')
    
    ax2 = ax.twinx()
    ax2.plot(PIXEL_SIZES, mean_sharpness, 'rs-', label='Mean Sharpness')
    ax2.set_ylabel('Mean Sharpness', color='r')
    ax2.tick_params(axis='y', labelcolor='r')
    
    ax.set_title('Resolution Tradeoff')
    ax.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig(f'{output_prefix}_resolution_metrics.png', dpi=150)
    plt.show()
    
    print("\n" + "="*60)
    print("Resolution Study Summary")
    print("="*60)
    print(f"{'Pixel (cm)':<12} {'Mean CNR':<12} {'Mean Sharpness':<15}")
    print("-"*40)
    for pix, cnr, sharp in zip(PIXEL_SIZES, mean_cnr, mean_sharpness):
        print(f"{pix:<12.2f} {cnr:<12.2f} {sharp:<15.4f}")
    
    opt_idx = np.nanargmax(mean_cnr)
    print(f"\nOptimal pixel size (max CNR): {PIXEL_SIZES[opt_idx]} cm")

# ============================================================
# MAIN
# ============================================================

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python muon_resolution_chunked.py <hits_data.csv> <config1|config2>")
        sys.exit(1)
    
    filename = sys.argv[1]
    config_name = sys.argv[2].lower()
    
    if config_name == 'config1':
        config = CONFIG1
    elif config_name == 'config2':
        config = CONFIG2
    else:
        print("Config must be 'config1' or 'config2'")
        sys.exit(1)
    
    output_prefix = f"muon_{config_name}"
    
    print(f"Using {config['name']}")
    
    # First pass: extract scatter data
    scatter_df = process_file_to_scatter_data(filename)
    
    # Save intermediate data for reuse
    scatter_df.to_csv(f'{output_prefix}_scatter_data.csv', index=False)
    print(f"Saved intermediate data: {output_prefix}_scatter_data.csv")
    
    print("\nRunning resolution study...")
    plot_resolution_comparison(scatter_df, config, output_prefix)
    plot_resolution_metrics(scatter_df, config, output_prefix)
    
    print(f"\nSaved: {output_prefix}_resolution_images.png, {output_prefix}_resolution_metrics.png")
