%%

delete(instrfindall);
close all;
clear all;
clc;

% fprintf(s,'d');

% mode_option = input('which mode?','s');

%             mode =0; % RGB discrete
mode =0; % red mode GB discrete
plot_num = 2;
%             how_many = 20;
how_many = 100;
disp('RGB discrete');

num_data = how_many;

temp_mode =0;
end_color =0;

automode = 1;

figure(2)
set(0,'DefaultFigureWindowStyle','docked')

data = [];



%s = serial('/dev/cu.usbmodem51885501')
s = serial('/dev/cu.usbserial-A700eYR2') % uart
fopen(s)
fprintf(s,'d'); % return to default mode
fprintf(s,'c'); % return to default mode

step = 64 %64
color_step = 0.25

if(step ==64)
  color_arr = {'r',[0.75 0.25 0], [0.5 0.5 0],[0.25 0.75 0], 'g',[0 0.75 0.25],[0 0.5 0.5],[0 0.25 0.75],'b',[0.25 0 0.75],'m'}
  legend_str={'Red','RY','Yellow','YG','Green','GC','Cyan','CB','Blue','BV','Violet'};
end


line_style ={'-.','--'}
text_str={'red','green','blue','violet'};
text_color = {'r','b','g','m'};

pos_hor = [ 10 22 30 40];
pos_ver = 2.5;

idx_sample = 0;

idx = 1;

   while(idx<= plot_num) % from red to violet by 32 step

        flushinput(s);

        if(end_color == 0)
            end_color_cnt = 640;
        elseif(end_color == 1)
            end_color_cnt = 768;
        end

        if( mode ==0)
%             fprintf(s,'d');
            fprintf(s,'s');
            disp('mode = default')
            sample_num = num_data;

        elseif(mode <4 && mode >0)
            disp('mode = s')
            fprintf(s,'s');
            sprintf('RGB discrete mode')
            sample_num = num_data;


        end

        h{idx} = plot(1,0,'marker','.');
        if(mode ==4)
            if(idx ==1)
              set(h{idx},'color','k');
            else
              set(h{idx},'color','m');
            end
        end

        flushinput(s);

        if(mode>=0)
            for i=1:sample_num
                line = fgets(s);

                newdata = str2num(line);

                      if(numel(newdata)==4)
                        idx_sample = idx_sample +1;
                        if(idx_sample==1)
                            data =newdata;
                        else
                            data =[data;newdata];
                        end

                        mode = data(idx_sample,1);

                        if(mod(data(idx_sample,2),64)==0)
                            if(data(idx_sample,2)==0)
                                color_code = 1;
                            end

                        end


                        set(h{idx}, 'XData',data(1:idx_sample,2));
                        set(h{idx}, 'YData',data(1:idx_sample,4));
                        set(h{idx}, 'linewidth',1);

                        hold on;
                        grid on;
%                         ylim([0 5.1])
                        ymax = max(data(:,4));
                        ylim([0 ymax*1.5]);

                        title('Spectrum scanning','fontsize',40);
                        xlabel('Sample count','fontsize',30)
                        ylabel('voltage','fontsize',30)



                        if(data(idx_sample,2) ==1)
                            ht{1}= text(data(idx_sample,2),1.5,'red');
                            set(ht{1},'fontsize',20, 'color','r');
                        end

                        drawnow;

                        if(i ==  sample_num)
                          idx_sample = 0;
                        end
                     end % end if

            hold on
            grid on;
        end  % end of for

%{
        if(data(end,1) <= 4)
            if(mode ==1)
                legend_list =legend_str{4*idx-3};
            elseif(mode ==2 || mode ==3)
                legend_list =[legend_list {legend_str{4*idx-3}}];
            else
                if(idx ==1)
                    legend_list ={'No algae'};
                else
                    legend_list =[legend_list {'With algae'}];
                end
            end

        end

         if(mode>0 && mode <4)
            set(h{idx},'color',color_arr{color_code}); % RGB

         end

        hl = legend(legend_list);


        set(hl,'fontsize',20);
        set(hl,'location','southwest');
%}
        if(idx ==5)
            set(h{idx},'linestyle',line_style{1});
            set(h{idx},'linewidth',3);
        elseif(idx ==6)
            set(h{idx},'linestyle',line_style{2});
            set(h{idx},'linewidth',3);
        end

        %end  % end of for
    
    end % end of if mode ~=0

    sprintf('idx = %d,mode = %d',idx,mode)

    hold on;

    if(idx ==1)
        comp1 = data;
        before = data(:,4);
        smooth_data1 = smooth(data(:,4));


       plot(comp1(:,2),smooth_data1,'color','b','linewidth',3)  
    else
        comp2 = data;
        after = data(:,4);
        diff = before-after;
        smooth_data2 = smooth(data(:,4));

        plot(comp2(:,2),smooth_data2,'linewidth',3)  
    end
    hold on;
    

    smooth_data = smooth(data(:,3))
    
    reply = input('replace ready?','s');
    while(reply~='y')
        disp('continue');
    end

    flushinput(s);
     clear data;
    idx = idx +1;

end % end of for/while
fclose(s)

%%

axis auto
ht1= text(1,1.5,'red');

hold on;
ht2= text(256,1.5,'green');
ht3= text(512,1.5,'blue');
ht4= text(640,1.5,'violet');
set(ht1,'color','r','fontsize',20);
set(ht2,'color','g','fontsize',20);
set(ht3,'color','b','fontsize',20);
set(ht4,'color','m','fontsize',20);

title('Spectrum scanning','fontsize',40);
xlabel('Sample count','fontsize',30)
ylabel('voltage','fontsize',30)
legend('plane water','algae')


%%

clear all
delete(instrfindall);


