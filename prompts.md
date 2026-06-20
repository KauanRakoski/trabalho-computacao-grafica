# Prompts Record

1. Gemini, I am building a Crash Team Kart Racing Clone using OpenGL and C++. I wish to create the "Once Upon a Tire Map" using obj and textures (all within data/map folder). The map should have colisions, using our already designed AABB system. I need you to generate this map and its collisions. Organize code in modules to keep legibility if needed.
2. Follow the generated implementation plan and implement the map.
3. All textures are black and you can't really see the map. Fix it
4. The textures are still apearing black. Have you registered the images like in LoadShadersFromFiles?
5. Still all map textures are black. See if your program is reading them correctly and registering them. For example: `void LoadShadersFromFiles() { ... }`
6. Ok, nice. Now, make the collisions work, crash is stuck, i need him to be able to travel on the map
7. Nice, now we can drive the car. We still have a problem: as it seems crash always collides with the highest object. The track has some objects you should drive into (like a castle) but as we try to enter we are sent to the ceiling
8. The problem still persists. It seem some bounding boxes put crash above where he needs to be
9. Ok, in the root folder create a docs folder. Document the logic of the files you created (how the map is loaded, how the colisions are handled, etc.) on a map-logic.md file. Also, create a prompts-record.md file and put all my promts as are, numerated. Whenever I ask you something, you should add the prompt on that file.


10. "Can you add a second plane 45 degrees inclined right next to the original one?"

11. "Poderia modularizar a lógica de colisão já existente que utiliza de AABB criando um arquivo collisions.cpp para armazenar todos os testes de colisão?"

12. "Poderia adicionar o objeto box.obj com a textura box.jpg a cena atual?"

13. "Poderia realizar uma mudança no código para ao apertar a tecla 1 mudar para uma câmera em primeira pessoa?"

14. Ok, gemini. We have a serious problem: the kart does not collide with walls. The collisions with the ground are ocurring fine, but with walls there is no collision whatsoever. Try to fix this error, by making the collisions with both ground and walls work. Also, make it log the coordinates of whatever wall player is currently colliding - to test later if there are any ghost walls. Compile the code and make sure it runs before answering. If it does not work, fix until it does. 

Compilation and running commands: 
cmake --build build
cd bin/debug
./main.exe

15. Ok, at first we have an invisible wall at Wall collision at coords: (-8.525755, -3.626804, -23.450333)

16. Another invisible wall at Wall collision at coords: (-0.018161, -5.165146, -39.473209)

17. It is getting worse. I cant even get out of place now... Fix it, change whatever is necessary. Make something that actually works. It should not block player on ramps, only on vertical walls

18. Now he is going through walls and getting stuck at the ground. Also, climbing objects that he should not collide. Change everything if needed, but fix this behavior

19. The original was better. Go back to that version and build up from there...
