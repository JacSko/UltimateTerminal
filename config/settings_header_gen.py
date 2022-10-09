import json
import sys

if len(sys.argv) < 3:
    print("Please provide correct arguments!")
    sys.exit()

print("Starting header generator")
input_file_path = sys.argv[1]
output_file_path = sys.argv[2]
print("INPUT FILE: " + str(input_file_path))
print("OUTPUT FILE: " + str(output_file_path))


config_file = open(input_file_path)
output_file = open(output_file_path, "w")

json_config = json.load(config_file)

output_file.write("#ifndef _SETTINGS_CONFIG_H\n")
output_file.write("#define _SETTINGS_CONFIG_H\n\n")

output_file.write("/* ==========================================================*/\n")
output_file.write("/* ========== auto-generated file - do not modify! ==========*/\n")
output_file.write("/* ==========================================================*/\n")
output_file.write("/* ==== edit system_config.json and rebuild the target! =====*/\n")
output_file.write("/* ==========================================================*/\n\n\n")

output_file.write("#define SETTING_GROUPS\\\n")
for item in json_config:
    output_file.write("   DEF_SETTING_GROUP(" + str(item) + ", ")
    if type(json_config[item]) is int:
        output_file.write("uint32_t, " + str(json_config[item]))
    elif type(json_config[item]) is bool:
        value = "false"
        if json_config[item] is True:
            value = "true"
        output_file.write("bool, " + value)
    elif type(json_config[item]) is str:
        output_file.write("std::string, " + "\"" + str(json_config[item]) + "\"")
    output_file.write(") \\\n")

output_file.write("\n\n#endif")
output_file.close()
print("Header generator finished work!")