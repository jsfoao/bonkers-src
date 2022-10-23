#pragma once
#define CAT( A, B ) A ## B
#define SELECT( NAME, NUM ) CAT( NAME ## _, NUM )
#define GET_COUNT( _1, _2, _3, _4, _5, _6, COUNT, ... ) COUNT
#define VA_SIZE( ... ) GET_COUNT( __VA_ARGS__, 6, 5, 4, 3, 2, 1 )
#define VA_SELECT( NAME, ... ) SELECT( NAME, VA_SIZE(__VA_ARGS__) )(__VA_ARGS__)

// LOGGING
#define LOG( T ) UE_LOG(LogTemp, Display, TEXT(T))
#define LOGF( T, ... ) UE_LOG(LogTemp, Display, TEXT(T), #__VA_ARGS__)
#define LOG_WARNING( T ) UE_LOG(LogTemp, Warning, TEXT(T))
#define LOG_ERROR( T ) UE_LOG(LogTemp, Error, TEXT(T))

#define LOG_SCREEN( T ) GEngine->AddOnScreenDebugMessage(0, 0, FColor::White, T)
#define LOG_SCREENF( T, ... ) GEngine->AddOnScreenDebugMessage(0, 0, FColor::White, FString::Printf(TEXT(T), ##__VA_ARGS__))
#define LOG_SCREEN_COLOR( T, C) GEngine->AddOnScreenDebugMessage(0, 0, C, T)
#define LOG_SCREEN_TIME( T, S ) GEngine->AddOnScreenDebugMessage(0, S, FColor::White, T)

// DEBUG DRAWING
#define DEF_THICKNESS 3.f
#define DEF_SEGMENTS 20.f
#define DRAW_SPHERE( CENTER, RADIUS, COLOR ) DrawDebugSphere(GetWorld(), CENTER, RADIUS, DEF_SEGMENTS, COLOR, false, -1, 0, DEF_THICKNESS)
#define DRAW_SPHERE_FULL( CENTER, RADIUS, SEGMENTS, COLOR, THICK ) DrawDebugSphere(GetWorld(), CENTER, RADIUS, SEGMENTS, COLOR, false, -1, 0, DEF_THICKNESS)
#define DRAW_BOX( CENTER, BOX, COLOR ) DrawDebugBox(GetWorld(), CENTER, BOX, COLOR, false, -1, 0, 2.f)
#define DRAW_BOX_FULL( CENTER, BOX, COLOR, THICK ) DrawDebugBox(GetWorld(), CENTER, BOX, COLOR, false, -1, 0, DEF_THICKNESS)
#define DRAW_LINE( START, END, COLOR ) DrawDebugLine(GetWorld(), START, END, COLOR, false, -1, 0, 2.f)
#define DRAW_LINE_FULL( START, END, COLOR, THICK ) DrawDebugLine(GetWorld(), START, END, COLOR, false, -1, 0, DEF_THICKNESS)

