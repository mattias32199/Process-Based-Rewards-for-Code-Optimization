# config.py
from dataclasses import dataclass
from typing import Optional
import torch

"""
TrainerConfig
|---EngineConfig
|        |-ModelConfig
|        |-LoraConfig
|


"""

@dataclass
class ModelConfig:
    """
    Config for initalizing FastLanguageModel (pretrained model)
        engine.py
    """
    model_name: str = "Qwen/Qwen2.5-Coder-1.5B-Instruct"
    max_seq_length: int = 2048
    dtype = torch.float16 # https://arxiv.org/pdf/2510.26788




@dataclass
class LoraConfig:
    """
    Config for initializing Lora weights (unsloth)
        engine.py
    """
    lora_rank: int = 64
    lora_dropout: float = 0.0
    bias: str = 'none'
    random_state: int = 3407


@dataclass
class EngineConfig:
    """
    Config for Engine
        engine.py
    """
    model: ModelConfig
    lora: LoraConfig
    lr: float = 1e-5
    weight_decay: float = 0.01
    debug: bool = False
    gpu_memory_utilization: float = 0.9


@dataclass
class GSPOConfig: # ?
    clip_ratio: float = 0.2
    clip_ratio_low: Optional[float] = None
    clip_ratio_high: Optional[float] = None

@dataclass
class TrainerConfig:
    engine: EngineConfig
    gspo: GSPOConfig
    epochs: int = 1
    max_turns: int = 4
    parallel_trajectories: int = 2
    top_p: float = 0.9
    max_new_tokens: int = 256
    temperature: float = 0.7
    use_cot: bool = False
    debug: bool = False
    vram_verbose: bool = False
    save_dir: str | None = None
    mini_batch_size: int = 8
