/*
    f_workspc: Workspace persintence, save and load
    module

    VincebusRiveruptum
*/

#include "files.h"

void f_saveWorkspace(Workspace *workspace, char *filename){
    FILE *fp = fopen(filename, "wb");
    Node *wndNode = NULL, *taNode = NULL;
    EditorWindow *wnd = NULL;
    TextArea *textArea = NULL;
    File *file = NULL;

    if(
        !workspace
    ){
        logger("[f_saveWorkspace] Warning: No valid workspace instance");
        return;
    }
    
    if(!filename){
        logger("[f_saveWorkspace] Warning: No valid workspace filename");
        return;
    }

    /* Workspace root object */
    fwrite(&workspace, sizeof(Workspace),1,fp);

    /* Window list*/
    /* windowList len */
    fwrite(workspace->windowList->length, sizeof(int), 1, fp);

    wndNode = workspace->windowList->firstNode;

    while(wndNode){
        wnd = (EditorWindow*)wndNode->data;

        if(!wnd){
            logger("[f_saveWorkspace] Error: Invalid wnd is NULL.");
            exit(1);
        }
        
        /* EditorWindow data */
        fwrite(&wnd, sizeof(EditorWindow),1,fp);

        /* textAreaList len */
        fwrite(wnd->textAreaList->length, sizeof(int),1,fp);

        taNode = wnd->textAreaList->firstNode;

        while(taNode){
            textArea = (TextArea*)taNode->data;

            if(!textArea){
                logger("[f_saveWorkspace] Error: Invalid textArea is NULl.");
                exit(1);
            }
            
            /* Text area name and len */
            fwrite(strlen(textArea->file->name), sizeof(int),1,fp);
            fwrite(&(textArea->file->name), strlen(textArea->file->name)*sizeof(char),1,fp);

            fwrite(&(textArea), sizeof(TextArea),1,fp);
            fwrite(&(textArea->file), sizeof(File),1,fp);

            taNode = taNode->next;
        }

        wndNode = wndNode->next;
    }

    fclose(fp);

    return;
}

Workspace *f_loadWorkspace(char *filename){
    char filename[255] = { 0 };       /* do not hardcode this! */
    int wndLen = 0, taLen = 0, i = 0, j = 0, w = 0;
    Node *wndNode = NULL, *taNode = NULL;
    EditorWindow *wnd = NULL;
    TextArea *textArea, *taTemp = NULL;
    File *file = NULL;
    Workspace *workspace = NULL;
    FILE *fp = fopen(filename, "rb");

    if(!filename){
        logger("[f_saveWorkspace] Warning: No valid workspace filename");
        return;
    }
    
    workspace = f_createWorkspace();

    /* Workspace root object */
    fread(&workspace, sizeof(Workspace),1,fp);   

    /* Window list*/
    /* windowList len */
    fread(wndLen, sizeof(int), 1, fp);

    wndNode = workspace->windowList->firstNode;

    while(i < wndLen){
        wnd = f_createWindow();
        
        if(!wnd){
            logger("[f_saveWorkspace] Error: Invalid wnd is NULL.");
            exit(1);
        }
        
        /* EditorWindow data */
        fread(&wnd, sizeof(EditorWindow), 1, fp);

        /* textAreaList len */
        fread(taLen, sizeof(int),1,fp);

        while(j < taLen){
            textArea = f_createTextArea();
            
            /* temp buffer */
            taTemp = (TextArea*)malloc(sizeof(TextArea));

            /* Text area name and len */
            memset(filename, 255, 0);
            fread(filenameLen, sizeof(int),1,fp);
            fread(&filename, sizeof(File),1,fp);

            fread(&taTemp, sizeof(TextArea),1,fp);

            f_setTextArea(textArea, taTemp);

            free(taTemp);

            textArea->file = f_readFile(filename);

            if(!textArea->file){
                logger("[f_loadWorkspace] Error : f_openFile returned NULL");
                exit(1);
            }

            j++;
        }

        i++;
    }

    fclose(fp);

    return;
}