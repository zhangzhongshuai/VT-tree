This is the YTStream dataset from MIRIS: Fast Object Track Queries in Video.

The shibuya, warsaw, and beach videos have been downloaded from the internet
and we do not own the copyright. We only provide them for non-commercial
research purposes.

Object detections are computed through a YOLOv3 model trained on hand-labeled
bounding box examples.

Object detections and tracks are stored in JSON files. Each file contains a
JSON list X, and X[i] is a list of the object detections in frame i. Each
detection (X[i][j]) is a JSON object with these keys:

    - left, top, right, bottom: bounding box coordinates
    - track ID: -1 in detection files, or the track ID in baseline tracker files
    - frame_idx: this should match the frame index i
