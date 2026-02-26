#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
	if (argc == 2) {
		// ---- EXTRACT HEADER FILE FEATURES ----
		// creates the file pointer
		FILE* file = fopen(argv[1], "r");
		if (file == NULL) {
			printf("something went wrong while reading the file\n");
			return -1;
		}

		// reads the ppm file format; P3 or P6
		int max_size = 1024;
		char* format = (char*)malloc(3 * sizeof(char));
		char* buffer = (char*)calloc(max_size, sizeof(char));
		fgets(buffer, max_size, file);
		sscanf(buffer, "%s", format);
		
		// gets the second row (if buffer[0] equals '#' it means there's a comment, so we skip the line)
		fgets(buffer, max_size, file);
		if (buffer[0] == '#') {
			fgets(buffer, max_size, file);
		}
		
		// from the second or third row gets the height and width
		int width, height;
		sscanf(buffer, "%d %d", &width, &height);
		
		// skips the third or fourth line
		fgets(buffer, max_size, file);
		free(buffer);

		/* ---- INITIALIZES SDL WINDOW TO RENDER IN ---- */
		
		// Initializes the SDL subsystem for image handling
		if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
			printf("Error in the initialization: %s\n", SDL_GetError());
			SDL_QuitSubSystem(SDL_INIT_VIDEO);
		}	

		// Creates the window
		SDL_Window* window = SDL_CreateWindow("Image", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_ALLOW_HIGHDPI);
		if (window == NULL) {
			printf("Error while creating the window: %s\n", SDL_GetError());
			SDL_DestroyWindow(window);
			SDL_QuitSubSystem(SDL_INIT_VIDEO);
			return -1;
		}

		// Gets the window surface
		SDL_Surface* surface = SDL_GetWindowSurface(window);
		if (surface == NULL) {
		 	printf("Error while getting the surface: %s\n", SDL_GetError());
		 	SDL_DestroyWindow(window);
		 	SDL_QuitSubSystem(SDL_INIT_VIDEO);
		 	return -1;
		}

		/* --- UPDATES THE WINDOW SURFACE BASED ON THE PPM FILE FORMAT ---- */
		// P3 format elaboration
		if (format[1] == '3') {
			printf("control p3\n");
			int dimension = width * height * 3;
			int* buffer2 = (int*)malloc(dimension * sizeof(int));

			// reads the value from the file stream
			for (int i = 0; i < dimension; i++) {
				fscanf(file, "%d", &buffer2[i]);
			}


			// prints the value on the surface (specific for retina display)
			int retina_width = width * 2;
			int retina_height = height * 2;
			SDL_Rect rect = {0, 0, 2, 2};
			int i = 0;
			for (int y = 0; y < retina_height; y+=2) {
				for (int x = 0; x < retina_width; x+=2) {
					rect.x = x;
					rect.y = y;
					SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, buffer2[i], buffer2[i+1], buffer2[i+2]));
					i += 3;
				}
			}
	
			free(buffer2);	
		}
		// P6 format elaboration
		else if (format[1] == '6') {
			printf("control p6\n");
			// prints the value on the surface (specific for retina display)
			int retina_width = width * 2;
			int retina_height = height * 2;
			SDL_Rect rect = {0, 0, 2, 2};
			Uint8 r, g, b;
			for (int y = 0; y < retina_height; y+=2) {
				for (int x = 0; x < retina_width; x+=2) {
					rect.x = x;
					rect.y = y;
					r = fgetc(file);
					g = fgetc(file);
					b = fgetc(file);
					SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, r, g, b));
				}
			}
		}

		// closes the file and updates the window surface
		fclose(file);
		SDL_UpdateWindowSurface(window);
		

		/* ---- RENDERING LOOP ---- */
		SDL_Event event;
		int quit = 0;

		while(!quit) {
			while(SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					quit = 1;
				}
			}
			SDL_Delay(16);
		}
			
		// Close the SDL SubSystem and deallocates the memory
		free(format);
		SDL_DestroyWindow(window);
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
		return 0;
	}

	else if (argc > 2) {
		printf("Too much arguments!\n");
		return -1;
	}

	else {
		printf("No arguments!\n");
		return -1;
	}
		
}
