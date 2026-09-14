#!/bin/bash
#SBATCH --time=10:00:00
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=16
#SBATCH --mem=32G
#SBATCH --output=LOG.log
#SBATCH --job-name=muon_analysis
#SBATCH --partition=physics
#SBATCH --mail-type=ALL
#SBATCH --mail-user=myemail

# -------- Load environment --------
module load anaconda3
source $(conda info --base)/etc/profile.d/conda.sh
conda activate rootenv

# -------- Run analysis scripts --------
echo "Starting analysis at $(date)"

# Main reconstruction
#echo "Running muon_analysis_chunked.py..."
#python muon_analysis_chunked.py hits_data.csv config1 > run_analysis.log 2>&1

# ASR metrics
#echo "Running muon_asr_chunked.py..."
#python muon_asr_chunked.py hits_data.csv config1 > run_asr.log 2>&1

# Quantitative Z correlation
#echo "Running muon_quantitative_chunked.py..."
#python muon_quantitative_chunked.py hits_data.csv config1 > run_quantitative.log 2>&1

# Resolution study
echo "Running muon_resolution_chunked.py..."
python muon_resolution_chunked.py hits_data.csv config1 > run_resolution.log 2>&1

echo "All analysis completed at $(date)"
