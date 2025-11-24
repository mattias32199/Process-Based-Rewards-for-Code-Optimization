import json
from typing import Dict, Any, List, Optional
from torch.utils.data import Dataset

import numpy as np
from collections import defaultdict
import random


class SimdBenchDataset(Dataset):
    """
    Dataset class for loading .jsonl files
    Stores only file offsets and lightweight metadata in memory.
    Loads full task data on-demand via __getitem__.
    """

    def __init__(
        self,
        jsonl_path: str,
        metadata_fields: Optional[List[str]] = None
    ):
        """
        Args:
            jsonl_path: Path to the JSONL file containing SimdBench tasks
            metadata_fields: List of fields to preload for fast access.
                           Defaults to ['task_id', 'kernel_type', 'architecture']
        """
        self.jsonl_path = jsonl_path
        self.metadata_fields = metadata_fields or ['task_id', 'kernel_type', 'architecture']

        # File offsets for each task (only ints in memory)
        self.offsets: List[int] = []
        # Lightweight metadata for sampling/filtering
        self.metadata: List[Dict[str, Any]] = []
        self._index_file()

    def _index_file(self):
        """Build index of file offsets and extract lightweight metadata."""
        with open(self.jsonl_path, 'r') as f:
            offset = 0
            for line in f:
                self.offsets.append(offset)

                # Parse just to extract metadata fields
                data = json.loads(line)
                meta = {field: data.get(field) for field in self.metadata_fields}
                self.metadata.append(meta)

                # Update offset for next line
                offset += len(line.encode('utf-8'))

    def __len__(self) -> int:
        return len(self.offsets)

    def __getitem__(self, idx: int) -> Dict[str, Any]:
        """Load full task data from disk."""
        if not 0 <= idx < len(self):
            raise IndexError(f"Index {idx} out of range [0, {len(self)})")

        with open(self.jsonl_path, 'r') as f:
            f.seek(self.offsets[idx])
            line = f.readline()
            return json.loads(line)

    def get_metadata(self, idx: int) -> Dict[str, Any]:
        """Get lightweight metadata without loading full task."""
        return self.metadata[idx]

    def filter_by_metadata(self, **kwargs) -> List[int]:
        """Return indices matching metadata criteria.

        Example:
            indices = dataset.filter_by_metadata(
                kernel_type='matmul',
                architecture='AVX'
            )
        """
        matching_indices = []
        for idx, meta in enumerate(self.metadata):
            if all(meta.get(k) == v for k, v in kwargs.items()):
                matching_indices.append(idx)
        return matching_indices





class SimdBenchDataLoader:
    """Memory-efficient stratified sampler for SIMD RL training.

    Ensures diverse kernel types within each batch for better learning.
    """

    def __init__(
        self,
        dataset,
        batch_size: int = 8,  # num_tasks per batch
        num_trajectories: int = 1,  # trajectories per task
        stratify_by: str = 'kernel_type',
        shuffle: bool = True,
        drop_last: bool = False,
        seed: Optional[int] = None
    ):
        """
        Args:
            dataset: SimdBenchDataset instance
            batch_size: Number of tasks per batch
            num_trajectories: Number of trajectories to generate per task
            stratify_by: Metadata field to stratify sampling on
            shuffle: Whether to shuffle indices within strata
            drop_last: Whether to drop incomplete final batch
            seed: Random seed for reproducibility
        """
        self.dataset = dataset
        self.batch_size = batch_size
        self.num_trajectories = num_trajectories
        self.stratify_by = stratify_by
        self.shuffle = shuffle
        self.drop_last = drop_last
        self.rng = np.random.RandomState(seed)

        # Build strata index (group indices by kernel_type)
        self._build_strata_index()

    def _build_strata_index(self):
        """Group dataset indices by stratification field."""
        self.strata = defaultdict(list)

        for idx in range(len(self.dataset)):
            meta = self.dataset.get_metadata(idx)
            stratum = meta.get(self.stratify_by, 'unknown')
            self.strata[stratum].append(idx)

        # Convert to numpy arrays for efficient sampling
        self.strata = {k: np.array(v) for k, v in self.strata.items()}
        self.stratum_names = list(self.strata.keys())

    def __len__(self) -> int:
        """Number of batches per epoch."""
        total_tasks = len(self.dataset)
        if self.drop_last:
            return total_tasks // self.batch_size
        return (total_tasks + self.batch_size - 1) // self.batch_size

    def __iter__(self):
        """Generate batches with stratified sampling."""
        # Shuffle indices within each stratum
        if self.shuffle:
            for stratum in self.strata.values():
                self.rng.shuffle(stratum)

        # Create stratified batches
        batches = self._create_stratified_batches()

        for batch_indices in batches:
            # Load tasks on-demand
            batch = [self.dataset[idx] for idx in batch_indices]

            # Expand for multiple trajectories
            # Return both the batch and indices for tracking
            yield {
                'tasks': batch,
                'indices': batch_indices,
                'num_trajectories': self.num_trajectories
            }

    def _create_stratified_batches(self) -> List[np.ndarray]:
        """Create batches ensuring diverse kernel types."""
        batches = []

        # Round-robin sampling across strata
        stratum_pointers = {k: 0 for k in self.stratum_names}

        while True:
            batch_indices = []

            # Sample from each stratum in round-robin fashion
            for _ in range(self.batch_size):
                # Find stratum with remaining samples
                available_strata = [
                    k for k in self.stratum_names
                    if stratum_pointers[k] < len(self.strata[k])
                ]

                if not available_strata:
                    break

                # Pick next stratum (cycles through types)
                stratum = available_strata[len(batch_indices) % len(available_strata)]

                # Get next index from this stratum
                idx = self.strata[stratum][stratum_pointers[stratum]]
                batch_indices.append(idx)
                stratum_pointers[stratum] += 1

            # Check if we have a complete batch
            if len(batch_indices) == 0:
                break

            if len(batch_indices) == self.batch_size:
                batches.append(np.array(batch_indices))
            elif not self.drop_last:
                batches.append(np.array(batch_indices))

            # Break if all strata exhausted
            if all(stratum_pointers[k] >= len(self.strata[k])
                   for k in self.stratum_names):
                break

        return batches
