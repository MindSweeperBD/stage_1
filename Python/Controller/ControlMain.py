import time
from Controller.ControlLayer import ControlLayer

def main():
    control = ControlLayer()
    print("Iniciando bucle de la Capa de Controller (Presiona Ctrl+C para detener)...")
    while True:
        control.control_pipeline_step()
        time.sleep(1)

if __name__ == "__main__":
    main()
