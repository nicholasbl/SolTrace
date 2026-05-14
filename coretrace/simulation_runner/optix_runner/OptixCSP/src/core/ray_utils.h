#pragma once

#include <cuda_runtime.h>
#include <cstdint>
#include <vector>

namespace OptixCSP
{

    struct HitRecord;

    /// Device scratch buffers required by gpu_compact_hit_buffer.
    /// Allocated once via allocate_compaction_scratch and reused across calls
    /// as long as num_rays and max_depth stay the same.
    struct CompactionScratch
    {
        uint32_t *d_count = nullptr;   // per-ray output record count
        uint32_t *d_offsets = nullptr; // exclusive prefix-sum of d_count
        uint32_t *d_has_hit = nullptr; // 1 if ray contributes records, else 0
        uint32_t *d_n_hit = nullptr;   // scalar: total hit rays
        void *d_scan_tmp = nullptr;    // CUB DeviceScan temp storage
        size_t scan_bytes = 0;
        void *d_red_tmp = nullptr; // CUB DeviceReduce temp storage
        size_t red_bytes = 0;
        HitRecord *d_compacted = nullptr; // worst-case compacted output (num_rays * max_depth)
    };

    /// Allocate all device scratch buffers for the given ray-buffer dimensions.
    /// Frees any previous allocation before reallocating.
    void allocate_compaction_scratch(CompactionScratch &scratch, uint32_t num_rays, uint32_t max_depth);

    /// Free all device scratch buffers and reset the struct to its default state.
    void free_compaction_scratch(CompactionScratch &scratch);

    /// GPU-side stream compaction of the raw hit buffer.
    /// Uses pre-allocated scratch buffers — no device allocations occur inside this call.
    /// Appends compacted HitRecords to @p host_out and the corresponding global ray indices
    /// (ray_offset + local_ray_index) to @p host_ray_ids (one entry per logical hit ray).
    /// @returns Number of rays that produced at least one non-CREATE hit.
    uint32_t gpu_compact_hit_buffer(
        const HitRecord *d_hit_buffer,
        uint32_t num_rays,
        uint32_t max_depth,
        uint32_t ray_offset,
        std::vector<HitRecord> &host_out,
        std::vector<uint32_t> &host_ray_ids,
        cudaStream_t stream,
        CompactionScratch &scratch);

} // namespace OptixCSP
