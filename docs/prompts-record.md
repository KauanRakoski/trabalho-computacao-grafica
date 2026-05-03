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
