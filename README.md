# ICP projekt

## Instalace závislostí

### Linux

#### GLEW
`apt-dependencies.txt` obsahuje seznam balíčků, které je potřeba nainstalovat pro správný chod programu. 

```
libopencv-dev
libglfw3-dev
libglm-dev
libglew-dev
v4l-utils
```

Pro instalaci závislostí spusťte následující příkaz:
```bash
./install.sh
```

### WSL

Vzhledem k tomu, že `./install.sh` skript pravděpodobně vybouchne, tak prověďte instalaci závislostí ručně. 

## Změna grafiky na NVIDIA

export MESA_D3D12_DEFAULT_ADAPTER_NAME=NVIDIA

#### GLEW
```bash
sudo apt install -y libopencv-dev libglfw3-dev libglm-dev libglew-dev 
mesa-utils
```

```bash
glxinfo | grep "OpenGL version"
```

`OpenGL version string: 4.6 (Compatibility Profile) Mesa 24.2.8-1ubuntu1~24.04.1`

#### X11
```bash
sudo apt-get install libxi-dev libxinerama-dev
```

### Win
#### ImGUI

To add ImGUI to your project:
1. Create new filter view: RMB on Solution explorer - project -> Add -> New filter
    - imgui 
2. follow: [Getting-Started](https://github.com/ocornut/imgui/wiki/Getting-Started#compilinglinking)
	- add existing items to imgui filter
3. add `$(ProjectDir)/imgui-master` to your Project properties -> VC++ directories -> Include directories
4. see: [imgui_manual](https://pthom.github.io/imgui_manual_online/manual/imgui_manual.html)

## Spuštění programu

Pro spuštění programu je potřeba zdrojoý kód zkompilovat a spustit.

```bash
make run
```