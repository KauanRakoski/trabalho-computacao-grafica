"Can you add a second plane 45 degrees inclined right next to the original one?"

"Poderia modularizar a lógica de colisão já existente que utiliza de AABB criando um arquivo collisions.cpp para armazenar todos os testes de colisão?"

"Poderia adicionar o objeto box.obj com a textura box.jpg a cena atual?"

"Poderia realizar uma mudança no código para ao apertar a tecla 1 mudar para uma câmera em primeira pessoa?"

Ok, gemini. We have a serious problem: the kart does not collide with walls. The collisions with the ground are ocurring fine, but with walls there is no collision whatsoever. Try to fix this error, by making the collisions with both ground and walls work. Also, make it log the coordinates of whatever wall player is currently colliding - to test later if there are any ghost walls. Compile the code and make sure it runs before answering. If it does not work, fix until it does. 

Compilation and running commands: 
cmake --build build
cd bin/debug
./main.exe

Ok, at first we have an invisible wall at Wall collision at coords: (-8.525755, -3.626804, -23.450333)

Another invisible wall at Wall collision at coords: (-0.018161, -5.165146, -39.473209)

It is getting worse. I cant even get out of place now... Fix it, change whatever is necessary. Make something that actually works. It should not block player on ramps, only on vertical walls

Now he is going through walls and getting stuck at the ground. Also, climbing objects that he should not collide. Change everything if needed, but fix this behavior

The original was better. Go back to that version and build up from there...

Cant you just try to remove the problematic pieces?

It is not colliding with walls

Invisible wall at: (-0.238248, -5.286940, -35.721790)

Ghost wall at: (0.304903, -4.478249, -41.622089)
