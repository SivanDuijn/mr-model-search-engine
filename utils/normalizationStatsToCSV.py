# converts normalizationStats.json to two CSVs, one without and one with preprocessed stats
import math
import sys
import json
import copy

f = open("normalizationStats.json")
stats = json.load(f)
f = open("files.json")
files = json.load(f)

unprocessedCSV = "file,nVertices,nFaces,position,aabbSize,angleX,angleY,angleZ,totalAngle,totalFlip\n"
processedCSV = "file,nVertices,nFaces,position,aabbSize,angleX,angleY,angleZ,totalAngle,totalFlip\n"

def addStatsToCSV(CSV, stats, file):
    CSV += file + "," + \
        str(stats["nVertices"]) + "," + \
        str(stats["nFaces"]) + "," + \
        str(stats["position"]) + "," + \
        str(stats["aabbSize"]) + "," + \
        str(stats["angleX"]) + "," + \
        str(stats["angleY"]) + "," + \
        str(stats["angleZ"]) + "," + \
        str(stats["totalAngle"]) + "," + \
        str(stats["totalFlip"]) + "\n"
    return CSV

for c in files:
    for file in files[c]:
        unprocessedCSV  = addStatsToCSV(unprocessedCSV, stats[file], file)
        ws = file.split(".")
        processedCSV  = addStatsToCSV(processedCSV, stats[ws[0] + "_processed." + ws[1]], ws[0] + "_processed." + ws[1])

output = open("normalizationStatsUnprocessed.csv", "w")
output.write(unprocessedCSV)
output.close()
output = open("normalizationStatsProcessed.csv", "w")
output.write(processedCSV)
output.close()


# create histograms
histogramsP = {
    "nVertices":  { "min": sys.float_info.max, "max": sys.float_info.min, "binSize": 1000,         "bins": []},
    "nFaces":     { "min": sys.float_info.max, "max": sys.float_info.min, "binSize": 2000,         "bins": []},
    "position":   { "min": sys.float_info.max, "max": sys.float_info.min, "binSize": 0.02,         "bins": []},
    "aabbSize":   { "min": sys.float_info.max, "max": sys.float_info.min, "binSize": 0.15,         "bins": []},
    "angleX":     { "min": sys.float_info.max, "max": sys.float_info.min, "binSize": math.pi / 32, "bins": []},
    "angleY":     { "min": sys.float_info.max, "max": sys.float_info.min, "binSize": math.pi / 32, "bins": []},
    "angleZ":     { "min": sys.float_info.max, "max": sys.float_info.min, "binSize": math.pi / 32, "bins": []},
    "totalAngle": { "min": sys.float_info.max, "max": sys.float_info.min, "binSize": math.pi / 4,  "bins": []},
    "totalFlip":  { "min": sys.float_info.max, "max": sys.float_info.min, "binSize": 1,            "bins": []},
}
histogramsUnp = copy.deepcopy(histogramsP)

# get max and min value
for c in files:
    for file in files[c]:
        ws = file.split(".")
        unpStats = stats[file]
        pStats = stats[ws[0] + "_processed." + ws[1]]
        for key in pStats:
            histogramsP[key]["min"] = pStats[key] if pStats[key] < histogramsP[key]["min"] else histogramsP[key]["min"]
            histogramsP[key]["min"] = unpStats[key] if unpStats[key] < histogramsP[key]["min"] else histogramsP[key]["min"]
            histogramsP[key]["max"] = pStats[key] if pStats[key] > histogramsP[key]["max"] else histogramsP[key]["max"]
            histogramsP[key]["max"] = unpStats[key] if unpStats[key] > histogramsP[key]["max"] else histogramsP[key]["max"]

# Initialize bins with zeros
for key in histogramsP:
    histogram = histogramsP[key]
    maxIndex = int((histogram["max"] - min(0, histogram["min"])) / histogram["binSize"])
    for i in range(0, maxIndex+1):
        histogram["bins"].append(0)
    
    histogramsUnp[key]["bins"] = copy.deepcopy(histogram["bins"])

# fill the bins
for c in files:
    for file in files[c]:
        ws = file.split(".")
        unpStats = stats[file]
        pStats = stats[ws[0] + "_processed." + ws[1]]
        for key in pStats:
            histogramP = histogramsP[key]
            i = int((abs(min(0, histogramP["min"])) + pStats[key]) / histogramP["binSize"])
            histogramP["bins"][i] += 1

            histogramUnp = histogramsUnp[key]
            i = int((abs(min(0, histogramP["min"])) + unpStats[key]) / histogramP["binSize"])
            histogramUnp["bins"][i] += 1

# convert bins to x y coord for bar chart
histogramsPCoords = {}
histogramsUnpCoords = {}
maxLenHist = sys.float_info.min
for key in histogramsP:
    values = []
    for i in range(0, len(histogramsP[key]["bins"])):
        values.append({"x": i*histogramsP[key]["binSize"], "y": histogramsP[key]["bins"][i]})
    histogramsPCoords[key] = values
    maxLenHist = len(values) if len(values) > maxLenHist else maxLenHist

    values = []
    for i in range(0, len(histogramsUnp[key]["bins"])):
        values.append({"x": i*histogramsP[key]["binSize"], "y": histogramsUnp[key]["bins"][i]})
    histogramsUnpCoords[key] = values

# Create histogram CSVs
# histogramPCSV = ""
# histogramUnpCSV = ""
# for key in histogramsP:
#     histogramPCSV += key + "-x," + key + "-y,"
# histogramPCSV = histogramPCSV[:-1] + "\n"
# histogramUnpCSV = histogramPCSV


# for i in range(0, maxLenHist):
#     for key in histogramsPCoords:
#         coordsP = histogramsPCoords[key]
#         coordsUnp = histogramsUnpCoords[key]
#         if i < len(coordsP):
#             histogramPCSV += str(round(coordsP[i]["x"] + min(0, histogramsP[key]["min"]),2)) + "," + str(coordsP[i]["y"]) + ","
#             histogramUnpCSV += str(round(coordsUnp[i]["x"] + min(0, histogramsP[key]["min"]),2)) + "," + str(coordsUnp[i]["y"]) + ","
#         elif i == len(coordsP):
#             histogramPCSV += str(round(coordsP[i-1]["x"] + min(0, histogramsP[key]["min"]) + histogramsP[key]["binSize"],2)) + ",0,"
#             histogramUnpCSV += str(round(coordsUnp[i-1]["x"] + min(0, histogramsP[key]["min"]) + histogramsP[key]["binSize"],2)) + ",0,"
#         else:
#             histogramPCSV += ",,"
#             histogramUnpCSV += ",,"
            
#     histogramPCSV = histogramPCSV[:-1] + "\n"
#     histogramUnpCSV = histogramUnpCSV[:-1] + "\n"

# output = open("histogramNormStatsP.csv", "w")
# output.write(histogramPCSV)
# output.close()
# output = open("histogramNormStatsUnp.csv", "w")
# output.write(histogramUnpCSV)
# output.close()

out = ""
out += "UNPROCESSED:\n"
for key in histogramsPCoords:
    coords = histogramsUnpCoords[key]
    out += key + ": "
    for i in range(0, len(histogramsP[key]["bins"])):
        out += "(" + str(round(coords[i]["x"] + min(0, histogramsP[key]["min"]),2)) + "," + str(coords[i]["y"]) + ") "
    out += "(" + str(round(coords[i]["x"] + min(0, histogramsP[key]["min"]) + histogramsP[key]["binSize"],2)) + ",0)\n"
    out += key + "-ticks: "
    for i in range(0, len(histogramsP[key]["bins"])):
        out += str(round(coords[i]["x"] + min(0, histogramsP[key]["min"]),2)) + ","
    out += str(round(coords[i]["x"] + min(0, histogramsP[key]["min"]) + histogramsP[key]["binSize"],2)) + "\n"

angles = ["angleX", "angleY", "angleZ"]
for i in range(0, len(angles)):
    angle = angles[i]
    coords = histogramsUnpCoords[angle]
    out += angle + "-combined: "
    for j in range(0, len(coords)):
        out += "(" + str(round(i*histogramsP[angle]["binSize"]/3 + coords[j]["x"] + min(0, histogramsP[angle]["min"]),2)) + "," + str(coords[j]["y"]) + ") "
        out += "(" + str(round(i*histogramsP[angle]["binSize"]/3 + histogramsP[angle]["binSize"]/3 + coords[j]["x"] + min(0, histogramsP[angle]["min"]),2)) + ",0) " 
    out += "(" + str(round(coords[j]["x"] + min(0, histogramsP[angle]["min"]) + histogramsP[angle]["binSize"],2)) + ",0)\n"
    out += angle + "-ticks: "
    for j in range(0, len(coords)):
        out += str(round(coords[j]["x"] + min(0, histogramsP[angle]["min"]),2)) + ","
    out += str(round(coords[j]["x"] + min(0, histogramsP[angle]["min"]) + histogramsP[angle]["binSize"],2)) + "\n"

out += "\nPROCESSED:\n"
for key in histogramsPCoords:
    coords = histogramsPCoords[key]
    out += key + ": "
    for i in range(0, len(histogramsP[key]["bins"])):
        out += "(" + str(round(coords[i]["x"] + min(0, histogramsP[key]["min"]),2)) + "," + str(coords[i]["y"]) + ") "
    out += "(" + str(round(coords[i]["x"] + min(0, histogramsP[key]["min"]) + histogramsP[key]["binSize"],2)) + ",0)\n"
    out += key + "-ticks: "
    for i in range(0, len(histogramsP[key]["bins"])):
        out += str(round(coords[i]["x"] + min(0, histogramsP[key]["min"]),2)) + ","
    out += str(round(coords[i]["x"] + min(0, histogramsP[key]["min"]) + histogramsP[key]["binSize"],2)) + "\n"

for i in range(0, len(angles)):
    angle = angles[i]
    coords = histogramsPCoords[angle]
    out += angle + "-combined: "
    for j in range(0, len(coords)):
        out += "(" + str(round(i*histogramsP[angle]["binSize"]/3 + coords[j]["x"] + min(0, histogramsP[angle]["min"]),2)) + "," + str(coords[j]["y"]) + ") "
        out += "(" + str(round(i*histogramsP[angle]["binSize"]/3 + histogramsP[angle]["binSize"]/3 + coords[j]["x"] + min(0, histogramsP[angle]["min"]),2)) + ",0) " 
    out += "(" + str(round(coords[j]["x"] + min(0, histogramsP[angle]["min"]) + histogramsP[angle]["binSize"],2)) + ",0)\n"
    out += angle + "-ticks: "
    for j in range(0, len(coords)):
        out += str(round(coords[j]["x"] + min(0, histogramsP[angle]["min"]),2)) + ","
    out += str(round(coords[j]["x"] + min(0, histogramsP[angle]["min"]) + histogramsP[angle]["binSize"],2)) + "\n"

output = open("histogramNormStats.txt", "w")
output.write(out)
output.close()