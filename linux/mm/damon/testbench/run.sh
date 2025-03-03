#!/usr/bin/env bash
sudo damo record -o damon.data $(pidof masim)
sudo damo report heatmap --output access_pattern_heatmap.png