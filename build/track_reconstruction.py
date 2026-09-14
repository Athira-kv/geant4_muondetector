#!/usr/bin/env python3
"""
Track reconstruction for muon tomography
Reconstructs incoming and outgoing tracks, calculates scattering angles
"""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy import stats
from dataclasses import dataclass
from typing import List, Tuple

@dataclass
class Track:
    """Represents a reconstructed track"""
    x0: float  # position at z=0
    y0: float
    theta_x: float  # angle in x-z plane (radians)
    theta_y: float  # angle in y-z plane (radians)
    chi2: float
    hits: List[Tuple[float, float, float]]  # (x, y, z) positions

def fit_track(hits):
    """
    Fit a straight line track to hit positions
    Returns Track object
    """
    if len(hits) < 2:
        return None
    
    hits = np.array(hits)
    x_hits = hits[:, 0]
    y_hits = hits[:, 1]
    z_hits = hits[:, 2]
    
    # Fit x vs z
    slope_x, intercept_x, r_x, p_x, std_err_x = stats.linregress(z_hits, x_hits)
    
    # Fit y vs z
    slope_y, intercept_y, r_y, p_y, std_err_y = stats.linregress(z_hits, y_hits)
    
    # Calculate chi-squared (simplified)
    x_pred = slope_x * z_hits + intercept_x
    y_pred = slope_y * z_hits + intercept_y
    chi2_x = np.sum((x_hits - x_pred)**2)
    chi2_y = np.sum((y_hits - y_pred)**2)
    chi2 = chi2_x + chi2_y
    
    # Calculate angles (in radians)
    theta_x = np.arctan(slope_x)
    theta_y = np.arctan(slope_y)
    
    # Position at z=0
    x0 = intercept_x
    y0 = intercept_y
    
    return Track(x0, y0, theta_x, theta_y, chi2, hits.tolist())

def reconstruct_event(event_data):
    """
    Reconstruct incoming and outgoing tracks for a single event
    """
    # Separate hits by detector
    dc1_hits = event_data[event_data['detector_type'] == 2][['x_cm', 'y_cm', 'z_cm']].values
    dc2_hits = event_data[event_data['detector_type'] == 3][['x_cm', 'y_cm', 'z_cm']].values
    dc3_hits = event_data[event_data['detector_type'] == 4][['x_cm', 'y_cm', 'z_cm']].values
    dc4_hits = event_data[event_data['detector_type'] == 5][['x_cm', 'y_cm', 'z_cm']].values
    
    # Combine upper chambers (incoming track)
    incoming_hits = np.vstack([dc1_hits, dc2_hits]) if len(dc1_hits) > 0 and len(dc2_hits) > 0 else []
    
    # Combine lower chambers (outgoing track)
    outgoing_hits = np.vstack([dc3_hits, dc4_hits]) if len(dc3_hits) > 0 and len(dc4_hits) > 0 else []
    
    if len(incoming_hits) < 2 or len(outgoing_hits) < 2:
        return None, None
    
    # Fit tracks
    incoming_track = fit_track(incoming_hits)
    outgoing_track = fit_track(outgoing_hits)
    
    return incoming_track, outgoing_track

def calculate_scattering_angle(incoming_track, outgoing_track):
    """
    Calculate scattering angle between incoming and outgoing tracks
    Returns scattering angles in x and y planes (radians), and total 3D angle
    """
    if incoming_track is None or outgoing_track is None:
        return None, None, None
    
    # Scattering angles (change in direction)
    scatter_x = outgoing_track.theta_x - incoming_track.theta_x
    scatter_y = outgoing_track.theta_y - incoming_track.theta_y
    
    # Total scattering angle (3D)
    scatter_total = np.sqrt(scatter_x**2 + scatter_y**2)
    
    return scatter_x, scatter_y, scatter_total

def calculate_poca(incoming_track, outgoing_track):
    """
    Calculate Point of Closest Approach between incoming and outgoing tracks
    This is where we estimate the scattering occurred (in the object)
    """
    if incoming_track is None or outgoing_track is None:
        return None, None, None
    
    # For simplicity, approximate POCA as the midpoint between track projections at z=0
    # This is reasonable for small scattering angles
    
    x_poca = (incoming_track.x0 + outgoing_track.x0) / 2
    y_poca = (incoming_track.y0 + outgoing_track.y0) / 2
    z_poca = 0  # Object is at z=0
    
    return x_poca, y_poca, z_poca

# Main analysis
print("=== Track Reconstruction and Scattering Analysis ===\n")

# Load data
df = pd.read_csv('hits_data.csv')
# Keep only muons (mu- or mu+)
df = df[df['particle'].isin(['mu-', 'mu+'])]

print(f"Loaded {len(df)} hits from {df['event_id'].nunique()} events\n")

# Reconstruct all events
results = []

for event_id in df['event_id'].unique():
    event_data = df[df['event_id'] == event_id]
    
    # Reconstruct tracks
    incoming, outgoing = reconstruct_event(event_data)
    
    if incoming is None or outgoing is None:
        continue
    
    # Calculate scattering
    scatter_x, scatter_y, scatter_total = calculate_scattering_angle(incoming, outgoing)
    
    # Calculate POCA (where scattering occurred)
    x_poca, y_poca, z_poca = calculate_poca(incoming, outgoing)
    
    results.append({
        'event_id': event_id,
        'incoming_x0': incoming.x0,
        'incoming_y0': incoming.y0,
        'incoming_theta_x': incoming.theta_x,
        'incoming_theta_y': incoming.theta_y,
        'outgoing_x0': outgoing.x0,
        'outgoing_y0': outgoing.y0,
        'outgoing_theta_x': outgoing.theta_x,
        'outgoing_theta_y': outgoing.theta_y,
        'scatter_x_mrad': scatter_x * 1000,  # Convert to milliradians
        'scatter_y_mrad': scatter_y * 1000,
        'scatter_total_mrad': scatter_total * 1000,
        'poca_x_cm': x_poca,
        'poca_y_cm': y_poca,
        'poca_z_cm': z_poca,
        'incoming_chi2': incoming.chi2,
        'outgoing_chi2': outgoing.chi2
    })

tracks_df = pd.DataFrame(results)
print(f"Successfully reconstructed {len(tracks_df)} events\n")

# Save reconstructed tracks
tracks_df.to_csv('reconstructed_tracks.csv', index=False)
print("Saved to: reconstructed_tracks.csv\n")

# Statistics
print("=== Scattering Statistics ===")
print(f"Mean scattering angle: {tracks_df['scatter_total_mrad'].mean():.2f} ± {tracks_df['scatter_total_mrad'].std():.2f} mrad")
print(f"RMS scattering angle: {np.sqrt(np.mean(tracks_df['scatter_total_mrad']**2)):.2f} mrad")
print(f"Max scattering angle: {tracks_df['scatter_total_mrad'].max():.2f} mrad")
print()

# Plotting
fig, axes = plt.subplots(2, 3, figsize=(18, 12))

# 1. Scattering angle distribution
axes[0, 0].hist(tracks_df['scatter_total_mrad'], bins=50, alpha=0.7, edgecolor='black')
axes[0, 0].set_xlabel('Total Scattering Angle (mrad)', fontsize=12)
axes[0, 0].set_ylabel('Counts', fontsize=12)
axes[0, 0].set_title('Scattering Angle Distribution', fontsize=13)
axes[0, 0].axvline(tracks_df['scatter_total_mrad'].mean(), color='r', 
                   linestyle='--', label=f'Mean: {tracks_df["scatter_total_mrad"].mean():.1f} mrad')
axes[0, 0].legend()
axes[0, 0].grid(True, alpha=0.3)

# 2. Scattering in X vs Y
axes[0, 1].scatter(tracks_df['scatter_x_mrad'], tracks_df['scatter_y_mrad'], 
                   alpha=0.3, s=10)
axes[0, 1].set_xlabel('Scattering Angle X (mrad)', fontsize=12)
axes[0, 1].set_ylabel('Scattering Angle Y (mrad)', fontsize=12)
axes[0, 1].set_title('Scattering: X vs Y Components', fontsize=13)
axes[0, 1].axhline(0, color='k', linestyle='--', alpha=0.3)
axes[0, 1].axvline(0, color='k', linestyle='--', alpha=0.3)
axes[0, 1].grid(True, alpha=0.3)
axes[0, 1].axis('equal')

# 3. POCA positions (where scattering occurred)
h = axes[0, 2].hist2d(tracks_df['poca_x_cm'], tracks_df['poca_y_cm'], 
                      bins=30, cmap='viridis')
axes[0, 2].set_xlabel('X Position (cm)', fontsize=12)
axes[0, 2].set_ylabel('Y Position (cm)', fontsize=12)
axes[0, 2].set_title('Muon Intersection Points at Object', fontsize=13)
plt.colorbar(h[3], ax=axes[0, 2], label='Counts')
axes[0, 2].set_aspect('equal')

# Draw object outline
from matplotlib.patches import Rectangle, Circle
block = Rectangle((-5, -5), 10, 10, fill=False, edgecolor='red', linewidth=2, label='Lead block')
cavity = Circle((2, 0), 2, fill=False, edgecolor='yellow', linewidth=2, label='Air cavity')
axes[0, 2].add_patch(block)
axes[0, 2].add_patch(cavity)
axes[0, 2].legend()

# 4. Incoming track angles
axes[1, 0].hist2d(tracks_df['incoming_theta_x'] * 1000, 
                  tracks_df['incoming_theta_y'] * 1000, bins=30, cmap='Blues')
axes[1, 0].set_xlabel('θ_x (mrad)', fontsize=12)
axes[1, 0].set_ylabel('θ_y (mrad)', fontsize=12)
axes[1, 0].set_title('Incoming Track Angles', fontsize=13)

# 5. Track quality (chi-squared)
axes[1, 1].hist(tracks_df['incoming_chi2'], bins=50, alpha=0.7, label='Incoming', edgecolor='black')
axes[1, 1].hist(tracks_df['outgoing_chi2'], bins=50, alpha=0.7, label='Outgoing', edgecolor='black')
axes[1, 1].set_xlabel('Track χ² (cm²)', fontsize=12)
axes[1, 1].set_ylabel('Counts', fontsize=12)
axes[1, 1].set_title('Track Fit Quality', fontsize=13)
axes[1, 1].legend()
axes[1, 1].set_yscale('log')
axes[1, 1].grid(True, alpha=0.3)

# 6. Scattering vs position (1D profile)
# Bin by X position and show mean scattering
x_bins = np.linspace(-10, 10, 21)
x_centers = (x_bins[:-1] + x_bins[1:]) / 2
scatter_profile = []

for i in range(len(x_bins)-1):
    mask = (tracks_df['poca_x_cm'] >= x_bins[i]) & (tracks_df['poca_x_cm'] < x_bins[i+1])
    if mask.sum() > 0:
        scatter_profile.append(tracks_df[mask]['scatter_total_mrad'].mean())
    else:
        scatter_profile.append(0)

axes[1, 2].plot(x_centers, scatter_profile, 'o-', linewidth=2, markersize=8)
axes[1, 2].set_xlabel('X Position (cm)', fontsize=12)
axes[1, 2].set_ylabel('Mean Scattering Angle (mrad)', fontsize=12)
axes[1, 2].set_title('Scattering Profile vs X Position', fontsize=13)
axes[1, 2].axvspan(-5, 5, alpha=0.2, color='gray', label='Lead block')
axes[1, 2].axvspan(0, 4, alpha=0.2, color='yellow', label='Cavity region')
axes[1, 2].legend()
axes[1, 2].grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig('track_reconstruction.png', dpi=150)
print("Plots saved to: track_reconstruction.png\n")

plt.show()
