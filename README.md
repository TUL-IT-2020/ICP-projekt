# ICP projekt

## Instalace závislostí

### Linux

#### GLEW
`apt-dependencies.txt` obsahuje seznam balíčků, které je potřeba nainstalovat pro správný chod programu. 

```txt
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

Ten také nainstaluje závsilosti `C++` knihoven:
```txt
nlohmann-json
```

### WSL
Pro instalaci závislostí spusťte následující příkaz:
```bash
./install.sh
```

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

### Windows

#### Změna grafiky na NVIDIA

`export MESA_D3D12_DEFAULT_ADAPTER_NAME=NVIDIA`

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

## Zdroje:
Textury a snipety kódu jsou převzaty z následujících zdrojů:
- [Fable Wolfenstein](https://github.com/JamesRandall/fsharp-wolfenstein)
- [Wolfenstein 3D Re-implementation in C++](https://github.com/mhamzaqayyum/wolf3d-reimpl-cpp)
- [Wolfenstein 3D C++](https://github.com/LeviMooreDev/Wolfenstein-3D-CPlusPlus)


## Tvorba mapy:
Legenda pro tvorbu mapy:
```txt
' ' = empty
. = empty

// malá abeceda
a = barel 1
b = bead
c = corpse 1
d = killed enemy
e = normal enemy (guard)
f = flag
g = gold
h = health 1
i = health 2
j = health 3
k = key
l = life
m = munition
n
o = barel 2
p = player start
q
r = gun 1
s = gun 2
t = table 1
u = table 2
v
w
x
y = chandelier_1
z = chandelier_2
// velká abeceda
A
B = blue wall
C = corpse 4
D = door
E = hidden door
F
G
H = Hans
I = lamp
J
K = knight
L = stove
M = spears
N
O
P = pillar
Q
R
S = stone 1
T = stone 2
U = stone 3
V = wood 2
W = wood 1
X = end level (exit/elevator)
Y = wood 3
Z
```



## TODO:
- [ ] Shader program hpp - přidat inline k metodám setUniform
- [x] Opravit pořadí vykreslování průhledných modelů.

```cpp
// remember:
// void draw(glm::vec3 const & offset = glm::vec3(0.0), glm::vec3 const & rotation = glm::vec3(0.0f))
//#######              DRAW             ###############
// option 1: draw object, no changes in position, rotation, scale, etc.
    scene.at("model_of_something").draw();
//#####################################################
// option 2: pass new position, that is changing in every frame (saving is useless)
    scene.at("model_of_something").draw(glm::vec3(0.0f),
                glm::vec3(0.0f, glm::radians(static_cast<float>(360*glfwGetTime())), 0.0f) );
//#####################################################
// option 2: compute and save new position, simple transformation
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        scene.at("model_of_something").rotation.y = glm::radians(static_cast<float>(360*glfwGetTime()));
    }
    scene.at("model_of_something").draw();
//#####################################################
// option 3: - for complex transformations: combine multiple transformations to model matrix  
// 			 - prepare model transformation locally, stack it with internal position
//           - see Model-extended.cpp for draw(glm::mat4 const & model_matrix)
    glm::mat4 m_m = glm::identity<glm::mat4>();
    m_m = glm::rotate(m_m, glm::radians(static_cast<float>(360*glfwGetTime())), glm::vec3(0.0f, 0.1f, 0.0f));
    m_m = glm::scale(m_m, glm::vec3(1.0f + glm::sin(glfwGetTime())));
    scene.at("model_of_something").draw(m_m);
//#####################################################
// option 4 (for dymamic object): derive new class - rotatingModel, modify update() method, so that
//                                it modifies member variable rotation itself
    scene.at("model_of_something").update(delta_t);
    scene.at("model_of_something").draw();
```