import bisect


class Spans:
    def __init__(self, ranges):
        merged = []
        for lo, hi in sorted((lo, hi) for lo, hi in ranges if lo < hi):
            if merged and lo <= merged[-1][1]:
                merged[-1][1] = max(merged[-1][1], hi)
            else:
                merged.append([lo, hi])
        self.starts = [lo for lo, _ in merged]
        self.ends = [hi for _, hi in merged]

    def __contains__(self, x):
        i = bisect.bisect_right(self.starts, x) - 1
        return i >= 0 and x < self.ends[i]
