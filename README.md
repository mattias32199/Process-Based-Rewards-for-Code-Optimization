# Process-Based-Rewards-for-Code-Optimization
*Course project for Intro to Deep Learning Fall 2025*
## Contributors
- myhollma@andrew.cmu.edu
- johnzhan@andrew.cmu.edu
- diacl@andrew.cmu.edu
- dharunmn@andrew.cmu.edu

## Branches
- Benchmark branch contains extraction and processing of SIMDBench
- Verification contains verifier classes (WIP)

## Related links
- [Baseline model outputs (Qwen2.5-Coder) pass@5](https://drive.google.com/file/d/1YyZxkFOjTxAkBqx40YXieJg3_A0r2-uk/view?usp=sharing)
- [Performance evaluation on baseline results](https://drive.google.com/file/d/1Y2f3s-xPwwVvYIdIHa3tsIbwCQU0KkfU/view?usp=sharing)


## setup

cd ~
wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
bash Miniconda3-latest-Linux-x86_64.sh

source ~/.bashrc

pip install torch

pip install -r requirements.txt