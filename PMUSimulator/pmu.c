/* ----------------------------------------------------------------------------- 
 * pmu.c
 * 
 * PMU Simulator - Phasor Measurement Unit Simulator
 *
 * Copyright (C) 2011-2012 Nitesh Pandit
 * Copyright (C) 2011-2012 Kedar V. Khandeparkar
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Authors: 
 *		Nitesh Pandit <panditnitesh@gmail.com>
 *		Kedar V. Khandeparkar <kedar.khandeparkar@gmail.com>
 *		Sancho Simmy Louis <sanchoatiitb@gmail.com>				
 *
 * ----------------------------------------------------------------------------- */


#include  <stdio.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include  <stdlib.h> 
#include  <string.h>
#include  <dirent.h>
#include  <errno.h>
#include  <signal.h>
#include  <sys/shm.h>
#include  <sys/types.h>
#include  <unistd.h>
#include  <sys/stat.h>
#include  <pthread.h>
#include  <errno.h>
#include  <gtk/gtk.h>
#include  <time.h>
#include  "PmuGui.h"
#include  "ServerFunction.h"
#include  "CfgGuiFunctions.h"
#include  "CfgFunction.h"
#include  "ShearedMemoryStructure.h"

/* Common fixed path for storage of few common files */
/*#define UI_FILE "/usr/local/share/PMU/pmu.xml"*/


/* ---------------------------------------------------------------- */
/*                   main program starts here                       */
/* ---------------------------------------------------------------- */

int main(int argc, char **argv)
{
	/* local variables */
	GtkBuilder *builder;
	GError     *error = NULL;

	int  ShmID, err;
	char *ptr1;
	struct stat st;
	//key_t MyKey;
    
    //MyKey = 12346;
    srand((unsigned)time(NULL));
    MyKey = rand();
	system("clear");

	pidLocal = fork();

	if (pidLocal == 0) 
	{
        //printf("process ID -- Server -- %ld\n",pidLocal); 
		start_server();
	}
	else 
	{      
        //printf("process ID -- Main -- %ld\n",pidLocal); 
		/* Main process for PMU Configuration Setup */
		usleep(5000);
		
        if (signal(SIGUSR1, SIGUSR1_handler) == SIG_ERR) 
        {
            printf("SIGUSR-1 install error\n");
            exit(1);
        }
        //MyKey   = 12346;                     /* obtain the shared memory */
        //p1.pid = getpid();
        //printf("Main PID %ld, key %d\n",p1.pid,MyKey);
		ShmID   = shmget(MyKey, sizeof(char), 0666);
		ShmPTR  = (struct P_id *) shmat(ShmID, NULL, 0);
        ShmPTR->pidMain = getpid();
        ShmPTR->statusMsgSet = 0;
		p1      = *ShmPTR;                 /* get process-a's ID       */

		/* Init GTK+ */
		gtk_init(&argc, &argv);		

		/* Create new GtkBuilder object */
		builder = gtk_builder_new();

        char* UI_FILE_DIR = getenv("iPDC_UI_DIR");
        char UI_FILE[50];
        strcpy(UI_FILE, UI_FILE_DIR);
        strcat(UI_FILE,"/pmu.xml"); 

		/* Add glade file to GtkBuilder */
		if(!gtk_builder_add_from_file(builder, UI_FILE, &error))
		{
			g_free(error);
			return(1);
		}

		/* Allocate data structure */
		pmu_data = g_slice_new(pmuStruct);

		/* Get objects from UI */
		/*#define GW(name) CH_GET_WIDGET(builder,name, pmu_data)*/
		#define GW(dname, bname) CH_GET_WIDGET(builder,bname, pmu_data, dname)
		     GW(Pmu_Simulator, window);
		     GW(create_cfg_button, button1);
		     GW(header_frm_button, button4);
		     GW(pmu_details_button, button2);
		     GW(stat_modification_button, button6);
		     GW(cfg_modification_button, button5);
		     GW(pmu_menubar, menubar1);
             GW(menuitem2, menuitem2);
		     GW(rights_lable, label1);
		     GW(admin_label, admin_label);
		     GW(about_menuitem, imagemenuitem10);
		     GW(exit_menuitem, imagemenuitem5);
		     GW(E_button, button7);
		     GW(manage_data_source, button3);
		     GW(pmu_properties, button2);
             GW(start_server,imagemenuitem1);
             GW(menu_setup_cfg, imagemenuitem6);
             GW(menu_data_source, imagemenuitem7);
             GW(menu_cfg_modify, imagemenuitem8);
             GW(menu_stat_modify, imagemenuitem9);
             GW(menu_header_frm, imagemenuitem11);
             //GW(text_view);
             GW(text_view1, image2);
             GW(open_cfg, imagemenuitem2);
             GW(time_lable, label2);
             GW(statusbar, statusbar1);
		     //GW(Pmu_Simulator);
		     //GW(create_cfg_button);
		     //GW(header_frm_button);
		     //GW(pmu_details_button);
		     //GW(stat_modification_button);
		     //GW(cfg_modification_button);
		     //GW(pmu_menubar);
             //GW(menuitem2);
		     //GW(rights_lable);
		     //GW(admin_label);
		     //GW(about_menuitem);
		     //GW(exit_menuitem);
		     //GW(E_button);
		     //GW(manage_data_source);
		     //GW(pmu_properties);
             //GW(start_server);
             //GW(menu_setup_cfg);
             //GW(menu_data_source);
             //GW(menu_cfg_modify);
             //GW(menu_stat_modify);
             //GW(menu_header_frm);
             //GW(text_view);
             //GW(text_view1);
             //GW(open_cfg);
             //GW(time_lable);
		#undef GW 

		/* Connect signal to builder */
		gtk_builder_connect_signals(builder, pmu_data);
		gtk_builder_connect_signals(builder, NULL);

          // Changes how a toplevel window deals with its size request and user resize attempts. 
          gtk_window_set_resizable (GTK_WINDOW (pmu_data->Pmu_Simulator), TRUE);
          gtk_window_set_position(GTK_WINDOW(pmu_data->Pmu_Simulator), GTK_WIN_POS_CENTER);
          strcpy(UI_FILE, UI_FILE_DIR);
          strcat(UI_FILE,"/logo.png"); 
          gtk_window_set_icon(GTK_WINDOW(pmu_data->Pmu_Simulator), create_pixbuf(UI_FILE));

		/* Set the Title of Main Window */
		gtk_window_set_title (GTK_WINDOW (pmu_data->Pmu_Simulator), "PMU SIMULATOR");

		/* Disable all buttons except create Configuration button because cfg is not present in the system */
		gtk_widget_set_sensitive(GTK_WIDGET(pmu_data->create_cfg_button), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(pmu_data->manage_data_source), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(pmu_data->header_frm_button), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(pmu_data->cfg_modification_button), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(pmu_data->stat_modification_button), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(pmu_data->pmu_details_button), FALSE);

		gtk_widget_set_visible(GTK_WIDGET(pmu_data->menuitem2), FALSE);
		gtk_widget_set_visible(GTK_WIDGET(pmu_data->pmu_properties), FALSE);
		gtk_widget_set_visible(GTK_WIDGET(pmu_data->open_cfg), FALSE);

		/* Get the user's name for storing the PMU Setup File */
          	ptr1 = malloc( 200*sizeof(char));
          	memset(ptr1,'\0',200);
          	strcpy(ptr1,getenv("HOME"));

		if (ptr1 == NULL)
		{
			printf("user not found\n");
			exit(1);
		}
		strcat(ptr1, "/iPDC");
        

		if(stat(ptr1,&st) == 0)
		{
			strcat(ptr1, "/PMU");

			if(stat(ptr1,&st) == 0)
			{
				strcat(pmuFolderPath, ptr1);

				FILE * output;
				int check;
				char buff[200], is_empty[100];

				strcpy(buff, "ls ");
				strcat(buff, pmuFolderPath);
				strcat(buff, " | wc -l");

				output = popen(buff,"r");
				fgets (is_empty, 100, output); //write to the char
				pclose (output);

				check = atoi(is_empty);

				if (check == 0) //No Setup files is present
				{
					/* Setup files is not present in the system, so start with building new PMU Setup file */
					pmu_server ();
				}
				else
				{
					gtk_widget_set_visible(GTK_WIDGET(pmu_data->open_cfg), TRUE);
				}
			}
			else
			{
				if (mkdir (ptr1, 0700))
				{
					printf ("cannot create directory `%s': %s\n", pmuFolderPath, strerror (errno));
					validation_result ("cannot create iPDC directory on proposed path!");
				}
				else
				{
					strcat(pmuFolderPath, ptr1);

					/* Setup files is not present in the system, so start with building new PMU Setup file */
					//pmu_server (); /* Commented out on 20170830 */
                    /* Commented out by Gopal, to suppress
                    annoying dialog at the begning */
				}
			}

            		strcpy(ptr1,getenv("HOME"));
            		strcat(ptr1,"/iPDC/DataDir");

			if(stat(ptr1,&st) != 0)
			{
                		if (mkdir (ptr1, 0700))
                		{
                    			printf("Cannot create directory '%s': %s\n", ptr1, strerror(errno));
                		}
            		}
		}
		else
		{
			if (mkdir (ptr1, 0700))
			{
				printf ("cannot create directory `%s': %s", pmuFolderPath, strerror (errno));
			}
			else
			{
				strcat(ptr1, "/PMU");

				if (mkdir (ptr1, 0700))
				{
					printf ("cannot create directory `%s': %s", pmuFolderPath, strerror (errno));
					validation_result ("cannot create PMU directory on proposed path!");
				}
				else
				{
					strcat(pmuFolderPath, ptr1);

					/* Setup files is not present in the system, so start with building new PMU Setup file */
					pmu_server ();
				}
			}
		}

		/* Decorate the GUI */
		pmu_colors();

		/* Signal handling for buttons on  Main PMU Window */
		g_signal_connect (pmu_data->create_cfg_button, "clicked", G_CALLBACK(cfg_create_function), NULL); 
		g_signal_connect (pmu_data->menu_setup_cfg, "activate", G_CALLBACK(cfg_create_function), NULL); 

		g_signal_connect (pmu_data->manage_data_source, "clicked", G_CALLBACK(manage_data_sources), NULL); 
		g_signal_connect (pmu_data->menu_data_source, "activate", G_CALLBACK(manage_data_sources), NULL); 

		g_signal_connect (pmu_data->header_frm_button, "clicked", G_CALLBACK(hdr_create_function), NULL); 
		g_signal_connect (pmu_data->menu_header_frm, "activate", G_CALLBACK(hdr_create_function), NULL); 

		g_signal_connect (pmu_data->stat_modification_button, "clicked", G_CALLBACK(stat_chng_options), NULL); 
		g_signal_connect (pmu_data->menu_stat_modify, "activate", G_CALLBACK(stat_chng_options), NULL); 

		g_signal_connect (pmu_data->cfg_modification_button, "clicked", G_CALLBACK(Change_pmu_configuration), NULL); 
		g_signal_connect (pmu_data->menu_cfg_modify, "activate", G_CALLBACK(Change_pmu_configuration), NULL); 

		g_signal_connect (pmu_data->pmu_details_button, "clicked", G_CALLBACK(show_pmu_details), NULL); 
		g_signal_connect (pmu_data->start_server, "activate", G_CALLBACK(pmu_server), NULL);
		g_signal_connect (pmu_data->open_cfg, "activate", G_CALLBACK(pmu_setup_file_selection), GTK_WINDOW(pmu_data->Pmu_Simulator));
		g_signal_connect (pmu_data->pmu_properties, "activate", G_CALLBACK(show_pmu_details), NULL);
		g_signal_connect (pmu_data->about_menuitem, "activate", G_CALLBACK(about_pmu), NULL);
		g_signal_connect (pmu_data->exit_menuitem, "activate", G_CALLBACK(destroy), NULL);
		g_signal_connect (pmu_data->E_button, "clicked", G_CALLBACK(destroy), NULL);
		g_signal_connect_swapped (pmu_data->Pmu_Simulator, "destroy", G_CALLBACK (destroy), NULL);

		/* Destroy builder, since we don't need it anymore */
		g_object_unref(G_OBJECT(builder));

		/* Show window. All other widgets are automatically shown by GtkBuilder */
		gtk_widget_show(pmu_data->Pmu_Simulator);

		pthread_t t;
		if((err = pthread_create(&t,NULL,display_time,NULL))) 
          {
			perror(strerror(err));  
			exit(1);
		}

		/* GTK Main */
		gtk_main();

		/* Free any allocated data */
		g_slice_free(pmuStruct, pmu_data);
	}
	return(0);
}

/**************************************** End of File *******************************************************/
