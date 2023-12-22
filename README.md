# TimmyDisco

This project is about Timmy, him and his bucket are at the Disco!

This project was completed to tackle tinyobj to load mesh but also learn to add color and lightning to a scene using C++, glfw and OpenGl.

My first task was to use texture for the mesh color, where i had to load obj mesh with texture coordinate information that was givne to me.
Furthermore, I loaded texture image using stb_image.h
Finally we used the texture color for rendering by passing the texture coordinate tbuffer to the vertex shader. In the fragment shader, i added uniform sampler2D myTexture; and in the main function of the gragment shader I used FragColor = texture(myTexture, UV); to retrieve texel color at tex coord UV, and use it for the final FragColor.

I followed regular rendering config for proj matrix, model matrix, window size, background color and so on.

My second task was to create shading with point light. 
I used the formula ambient light color * obj color + diffuse light color * obj color * max(0, n-1) to computer FragColor. I have used understanding of the Phong Model to do so.


Task 3 was about adding the actual spotlights and shading with spotlights. I had a specific diffuse color, attenuation, ambient color, position, spot direction and cutoff angle for each spotlight, and used a while loop to spin the spotlights on positive y-axis counterclockwise by theta each time.
