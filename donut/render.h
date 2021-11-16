#ifndef RENDER_H
#define RENDER_H

unsigned int RenderThread(void* lpThreadParameter);
void Draw(HDC hdc, RECT rc);
void StopDrawing();

#endif