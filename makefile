All: final_project
 
    final_project: main.o Bits_And_Files_Functions.o Segments_Functions.o
	gcc main.o Bits_And_Files_Functions.o Segments_Functions.o -o final_project
    main.o: main.c Segments_Functions.h Bits_And_Files_Functions.h 
	gcc -c main.c
    Bits_And_Files_Functions.o: Bits_And_Files_Functions.c Bits_And_Files_Functions.h Segments_Functions.h
	gcc -c Bits_And_Files_Functions.c
    Segments_Functions.o: Segments_Functions.c Bits_And_Files_Functions.h Segments_Functions.h
	gcc -c Segments_Functions.c
