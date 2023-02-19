import os
import sys

def main():
    if len(sys.argv) < 2:
        print("Usage: python script.py <dir_path>")
        sys.exit(1)
    dir_path = sys.argv[1]
    #print(dir_path)
    filename = "go1.wav"
    pwd = "/Users/stefanodalmas/Desktop/speech_commands_v0.02\ 2/"
    label = dir_path.split("/")[-1]
    
    for filename in os.listdir(dir_path):
        os.system("edge-impulse-uploader --category split --label "+label+" "+pwd+dir_path+"/"+filename)
        os.remove(dir_path+"/"+filename)
        
if __name__ == "__main__":
    main()
