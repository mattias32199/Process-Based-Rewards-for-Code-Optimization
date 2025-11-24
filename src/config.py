# config.py
from dataclasses import dataclass
from typing import Optional

@dataclass
class ModelConfig:
    model_name: str = "Qwen/Qwen2.5-Coder-1.5B-Instruct"
    max_seq_length: int = 2048

@dataclass
class ActorConfig:
    clip_ratio: float = 0.2
    clip_ratio_low: Optional[float] = None
    clip_ratio_high: Optional[float] = None

@dataclass
class TrainerConfig:
    model: ModelConfig
    actor: ActorConfig
    epochs: int = 1
    max_turns: int = 4
    parallel_trajectories: int = 2
    max_new_tokens: int = 256
    temperature: float = 0.7
    top_p: float = 0.9
    lr: float = 1e-5