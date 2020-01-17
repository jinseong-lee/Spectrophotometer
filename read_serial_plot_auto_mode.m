%%
while(1)
close gcf
delete(instrfindall);
clear data;
clear all;clc;
% close all;
% fclose(s)
%delete all;
% delete port;
mode =0;

temp_mode =0;
end_color =0;

figure(2)
set(0,'DefaultFigureWindowStyle','docked')

% data = [];


s = serial('/dev/cu.usbmodem51885501')
fopen(s)

% fprintf(s,'r');

color = {'k','r','g','b','m'}


 num_data = 160;
 
% while(mode<5)

for idx=1:5
    h{idx} = plot(1,0,'marker','.');

    hold on;
    grid on;
    ylim([0 5.1])

    title('Light intensity in 5V','fontsize',50);
    xlabel('Sample count','fontsize',30)
    ylabel('voltage','fontsize',30)
        
    if(end_color == 0)
        end_color_cnt = 640;
    elseif(end_color == 1)
        end_color_cnt = 768;
    end
    
%     test=input('press key M(mode) or S(scan)','s')
%     sprintf('%c',test);
% 
%     if(test == 'm')
%         mode = mode + 1; % 0:same light, 1: scan
%         num_data = 100;
%     elseif (test == 's')
%         mode = 4; % 0:same light, 1: scan
%         num_data = end_color_cnt;
%     else
%         mode = 0; % 0:same light, 1: scan
%     end
    
  
    if( mode ==0)
        fprintf(s,'d');
        disp('mode = default')
        sample_num = num_data;
        
    elseif(mode <4 && mode >0)
        disp('mode = m')
        fprintf(s,'m');
        sprintf('RGB discrete mode')
        sample_num = num_data;
    elseif(mode == 4)
%         num_data = end_color_cnt;
        num_data = num_data;
        disp('mode = s')
        fprintf(s,'s');
        sample_num = num_data;
        sprintf('scan mode')
    end

  
    
    if(mode <= 4 && mode >=0)
%         h = plot(1,0,'color','r','marker','.');
        



        for i=1:sample_num
            flushinput(s);
            line = fgets(s)
%             vol = str2num(fscanf(s));
%             [mode, vol] = str2num(fscanf(s,'%d %3.2f'));

%             if(vol == NaN)
            newdata = str2num(line);
%             if(~isnan(line))
            if(~isnan(newdata(1)) && ~isnan(newdata(1)))
                if(i==1)
                    data =newdata;
                else
                    data =[data;newdata];
                end
            
                set(h{idx}, 'XData',1:i)
                set(h{idx}, 'YData',data(1:i,2))
                set(h{idx}, 'linewidth',3)
                set(h{idx},'color',color{idx});
                drawnow;
            end
        end
   

        hold on
        
        margin = 1.1;
    
       
        grid on;

        mode = mode + 1; % 0:same light, 1: scan
        temp_mode = mode;
    
    end
    

end

legend('Ambient','R','G','B','RGB all scan')

fclose(s)
clear all
delete(instrfindall);
end
%%
%     legend('R','G','B','Scan')
      
%%

clear all
delete(instrfindall);
