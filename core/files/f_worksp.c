/*
    f_worksp.c: Editor Workspace state persistence module.

    VincebusRiveruptum
*/

#include "files.h"

/* 
    Checks whether the file already was instanced in the workspace.
    If found, reuse the shared File pointer; otherwise read from disk.
*/
static File *_checkFilePtr(Workspace *workspace, char *filename){
    Node *wndNode = NULL;
    Node *taNode = NULL;
    EditorWindow *wnd = NULL;
    TextArea *textArea = NULL;

    if(!filename || filename[0] == '\0'){
        return NULL;
    }

    if(workspace && workspace->windowList){
        wndNode = workspace->windowList->firstNode;
        while(wndNode){
            wnd = (EditorWindow*)wndNode->data;
            if(wnd && wnd->textAreaList){
                taNode = wnd->textAreaList->firstNode;
                while(taNode){
                    textArea = (TextArea*)taNode->data;
                    if(
                        textArea && 
                        textArea->file && 
                        textArea->file->name
                    ){
                        if(strcmp(textArea->file->name, filename) == 0){
                            return textArea->file;
                        }
                    }
                    taNode = taNode->next;
                }
            }
            wndNode = wndNode->next;
        }
    }

    return f_readFile(filename);
}

void f_saveWorkspace(Workspace *workspace, char *filename){
    int wndCount = 0;
    int taCount = 0;
    unsigned int nameLen = 0;
    long wndId = 0;
    long taId = 0;
    char *name = NULL;
    FILE *fp = NULL;
    Node *wndNode = NULL;
    Node *taNode = NULL;
    EditorWindow *wnd = NULL;
    TextArea *textArea = NULL;

    logger("[f_saveWorkspace] Info: Saving workspace %s", filename);

    if(!workspace){
        logger("[f_saveWorkspace] Warning: No valid workspace instance");
        return;
    }
    
    if(!filename){
        logger("[f_saveWorkspace] Warning: No valid workspace filename");
        return;
    }

    fp = fopen(filename, "wb");
    if(!fp){
        logger(
            "[f_saveWorkspace] Error: Could not open %s for writing", 
            filename
        );
        return;
    }

    /* Workspace root object */
    fwrite(workspace, sizeof(Workspace), 1, fp);

    /* Window list length */
    wndCount = (workspace->windowList) ? workspace->windowList->length : 0;
    fwrite(&wndCount, sizeof(int), 1, fp);

    wndNode = (workspace->windowList) ? workspace->windowList->firstNode : NULL;

    while(wndNode){
        wnd = (EditorWindow*)wndNode->data;

        if(!wnd){
            logger("[f_saveWorkspace] Error: Invalid wnd is NULL.");
            fclose(fp);
            exit(1);
        }
        
        /* EditorWindow pointer (used as ID) */
        wndId = (long)wnd;
        fwrite(&wndId, sizeof(long), 1, fp);

        /* EditorWindow data */
        fwrite(wnd, sizeof(EditorWindow), 1, fp);

        /* textAreaList length */
        taCount = (wnd->textAreaList) ? wnd->textAreaList->length : 0;
        fwrite(&taCount, sizeof(int), 1, fp);

        taNode = (wnd->textAreaList) ? wnd->textAreaList->firstNode : NULL;

        while(taNode){
            textArea = (TextArea*)taNode->data;

            if(!textArea){
                logger("[f_saveWorkspace] Error: Invalid textArea is NULL.");
                fclose(fp);
                exit(1);
            }
            
            /* Text area pointer (used as ID) */
            taId = (long)textArea;
            fwrite(&taId, sizeof(long), 1, fp);
            
            /* Text area file name and length */
            name = (textArea->file && textArea->file->name) 
                ? textArea->file->name 
                : "";
            nameLen = (unsigned int)strlen(name);

            fwrite(&nameLen, sizeof(unsigned int), 1, fp);
            if(nameLen > 0){
                fwrite(name, sizeof(char), nameLen, fp);
            }

            /* TextArea state data */
            fwrite(textArea, sizeof(TextArea), 1, fp);

            taNode = taNode->next;
        }

        wndNode = wndNode->next;
    }

    fclose(fp);

    logger(
        "[f_saveWorkspace] Info: Workspace %s saved successfully!", 
        filename
    );
}

Workspace *f_loadWorkspace(char *filename){
    int wndLen = 0;
    int taLen = 0;
    int i = 0;
    int j = 0;
    unsigned int filenameLen = 0;
    long wndId = 0;
    long taId = 0;
    long savedActiveWndId = 0;
    long savedActiveTaId = 0;
    char filenameBuffer[256];
    FILE *fp = NULL;
    List *tmpWndList = NULL;
    List *tmpTAList = NULL;
    EditorWindow *wnd = NULL;
    TextArea *textArea = NULL;
    TextArea *taTemp = NULL;
    Workspace *workspace = NULL;

    logger("[f_loadWorkspace] Info: Loading workspace %s", filename);

    if(!filename){
        logger("[f_loadWorkspace] Warning: No valid workspace filename, creating fresh one");
        return f_createWorkspace();
    }

    fp = fopen(filename, "rb");

    if(!fp){
        logger(
            "[f_loadWorkspace] Warning: Could not open workspace file, creating fresh one %s", 
            filename
        );
        return f_createWorkspace();
    }

    workspace = f_createWorkspace();
    
    if(!workspace){
        logger("[f_loadWorkspace] Error: Could not create workspace");
        fclose(fp);
        exit(1);
    }

    tmpWndList = workspace->windowList;

    /* Workspace root object */
    if(fread(workspace, sizeof(Workspace), 1, fp) != 1){
        logger("[f_loadWorkspace] Error: Failed reading workspace header");
        fclose(fp);
        return currentWorkspace;
    }

    savedActiveWndId = (long)workspace->currentWindow;
    workspace->windowList = tmpWndList;
    workspace->currentWindow = NULL;

    /* Window list length */
    if(fread(&wndLen, sizeof(int), 1, fp) != 1){
        logger("[f_loadWorkspace] Error: Failed reading window count");
        fclose(fp);
        return currentWorkspace;
    }

    logger("[f_loadWorkspace] Debug: windows to load: %d", wndLen);

    while(i < wndLen){
        wnd = f_createWindow();
        if(!wnd){
            logger("[f_loadWorkspace] Error: Invalid wnd is NULL.");
            fclose(fp);
            exit(1);
        }

        tmpTAList = wnd->textAreaList;

        /* EditorWindow ptr as id */
        fread(&wndId, sizeof(long), 1, fp);

        /* EditorWindow data */
        fread(wnd, sizeof(EditorWindow), 1, fp);

        savedActiveTaId = (long)wnd->textArea;
        wnd->textAreaList = tmpTAList;
        wnd->textArea = NULL;

        /* textAreaList length */
        fread(&taLen, sizeof(int), 1, fp);

        logger(
            "[f_loadWorkspace] Debug: textAreas to load in window #%d: %d", 
            i, 
            taLen
        );

        j = 0;
        while(j < taLen){
            taTemp = (TextArea*)malloc(sizeof(TextArea));
            if(!taTemp){
                logger("[f_loadWorkspace] Error: malloc taTemp failed");
                fclose(fp);
                exit(1);
            }

            /* TA pointer as ID */
            fread(&taId, sizeof(long), 1, fp);

            /* Text area name and len */
            memset(filenameBuffer, '\0', sizeof(filenameBuffer));
            fread(&filenameLen, sizeof(unsigned int), 1, fp);

            if(filenameLen >= sizeof(filenameBuffer)){
                filenameLen = sizeof(filenameBuffer) - 1;
            }

            if(filenameLen > 0){
                fread(filenameBuffer, sizeof(char), filenameLen, fp);
            }
            
            filenameBuffer[filenameLen] = '\0';

            textArea = f_createTextArea(filenameBuffer);
            if(!textArea){
                logger(
                    "[f_loadWorkspace] Error: f_createTextArea returned NULL for %s", 
                    filenameBuffer
                );
                free(taTemp);
                fclose(fp);
                exit(1);
            }

            fread(taTemp, sizeof(TextArea), 1, fp);
            f_setTextArea(textArea, taTemp);

            textArea->file = _checkFilePtr(workspace, filenameBuffer);
            if(!textArea->file){
                logger(
                    "[f_loadWorkspace] Error: Could not open file %s", 
                    filenameBuffer
                );
                free(taTemp);
                fclose(fp);
                exit(1);
            }

            f_refreshTextArea(textArea);
            addGenericNode(&(wnd->textAreaList), (void*)textArea, NULL);

            if(taId == savedActiveTaId || !wnd->textArea){
                wnd->textArea = textArea;
            }

            free(taTemp);
            j++;
        }

        if(!wnd->textArea && wnd->textAreaList && wnd->textAreaList->firstNode){
            wnd->textArea = (TextArea*)wnd->textAreaList->firstNode->data;
        }

        addGenericNode(&(workspace->windowList), (void*)wnd, NULL);

        if(wndId == savedActiveWndId || !workspace->currentWindow){
            workspace->currentWindow = wnd;
        }

        i++;
    }

    if(
        !workspace->currentWindow && 
        workspace->windowList && 
        workspace->windowList->firstNode
    ){
        workspace->currentWindow = 
            (EditorWindow*)workspace->windowList->firstNode->data;
    }

    fclose(fp);

    currentWorkspace = workspace;
    currentWindow = workspace->currentWindow;

    logger(
        "[f_loadWorkspace] Info: Workspace %s loaded successfully", 
        filename
    );

    return workspace;
}
