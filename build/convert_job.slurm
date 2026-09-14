#!/bin/bash
#SBATCH --job-name=convert
#SBATCH --time=3:00:00
#SBATCH --cpus-per-task=1
#SBATCH --mem=32G
#SBATCH --output=convert.log
#SBATCH --partition=physics

# Load conda (replace module name if needed)
module load anaconda3
source $(conda info --base)/etc/profile.d/conda.sh
conda activate rootenv 

python convert_to_parquet.py
