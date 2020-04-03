%%

delete(instrfindall);
close all;
clear all;
clc;

% fprintf(s,'d');

mode_option = input('which mode?','s');
        if(mode_option=='1')   
            mode =1; % RGB discrete
            plot_num = 3;
%             how_many = 20;
            how_many = 500;
             disp('RGB discrete');
        elseif(mode_option=='4')   
            mode =4; % RGB discrete
            plot_num = 2;
%             how_many = 350;
            how_many = 640;
            disp('RGB continuous scan');
        elseif(mode_option=='5')   
            mode =5; % step increase
            plot_num = 11;
            how_many = 50;
            disp('color step change');
        end

num_data = how_many;

temp_mode =0;
end_color =0;

automode = 1;

figure(2)
set(0,'DefaultFigureWindowStyle','docked')

data = [];



s = serial('/dev/cu.usbmodem51885501')
fopen(s)
fprintf(s,'d'); % return to default mode
fprintf(s,'c'); % return to default mode

step = 64 %64
color_step = 0.25

if(step ==64)
% color = {'r','g','b','k',[0.1 0.3 0.8],'m',[0.1 0.6 0.1]}
color_arr = {'r',[0.75 0.25 0], [0.5 0.5 0],[0.25 0.75 0], 'g',[0 0.75 0.25],[0 0.5 0.5],[0 0.25 0.75],'b',[0.25 0 0.75],'m'}
legend_str={'Red','RY','Yellow','YG','Green','GC','Cyan','CB','Blue','BV','Violet'}
end


% legend_str={'blank','water','low algae','green water'}
% legend_str={'red','green','blue','blank','water','algae','green water'}

line_style ={'-.','--'}
text_str={'red','green','blue','violet'}
text_color = {'r','b','g','m'}

    

% pos_hor = [ 50 2.5 230 275];
pos_hor = [ 10 22 30 40];
pos_ver = 2.5;

idx_sample = 0;

idx = 1;
% mode = 1;


%    while(idx<= 11) % from red to violet by 32 step
   while(idx<= plot_num) % from red to violet by 32 step

% for idx=1:5
%         for idx= 4:7
          
        flushinput(s); 
%         h{idx} = plot(1,0,'marker','.','color','r');
  
        if(end_color == 0)
            end_color_cnt = 640;
        elseif(end_color == 1)
            end_color_cnt = 768;
        end

%         if(idx == 5)
%            idx = idx + 1;
%         end
        
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
    %         num_data = end_color_cnt-200;
            num_data = how_many;

            disp('mode = s')
            fprintf(s,'s');
            sample_num = num_data;
            sprintf('scan mode')
        elseif(mode == 5)
            num_data = how_many;
    %         num_data = num_data;
            disp('mode = a')
            fprintf(s,'a');
            sample_num = num_data;
            sprintf('step mode')
        end

        h{idx} = plot(1,0,'marker','.');
        if(mode ==4)
            if(idx ==1)
              set(h{idx},'color','k');
            else
              set(h{idx},'color','m');
            end
        end
%                         h = animatedline('Marker','o');
            
        flushinput(s); 
        
        if(mode>0)
            for i=1:sample_num
                line = fgets(s);

                newdata = str2num(line)

%                       if(numel(newdata)==2 && newdata(1)==4)
                      if(numel(newdata)==4)
                        idx_sample = idx_sample +1;
                        if(idx_sample==1)
                            data =newdata;
                        else
                            data =[data;newdata];
                        end
                        newdata

                        
                        mode = data(idx_sample,1);
                        
                        if(mod(data(idx_sample,2),64)==0)
                            if(data(idx_sample,2)==0)
                                color_code = 1;
                            elseif(data(idx_sample,2)==64)
                                color_code = 2;
                            elseif(data(idx_sample,2)==128)
                                color_code = 3;                            
                            elseif(data(idx_sample,2)==192)
                                color_code = 4;                            
                            elseif(data(idx_sample,2)==256)
                                color_code = 5;                            
                            elseif(data(idx_sample,2)==320)
                                color_code = 6;                                                        
                            elseif(data(idx_sample,2)==384)
                                color_code = 7;                            
                            elseif(data(idx_sample,2)==448)
                                color_code = 8;                            
                            elseif(data(idx_sample,2)==512)
                                color_code = 9;                                                                                    
                            elseif(data(idx_sample,2)==576)
                                color_code = 10;                                                                                                                
                            elseif(data(idx_sample,2)==640)
                                color_code = 11;    
                            end
                        else
                            if(data(idx_sample,2)<256) % color_code
                                color = [256-data(idx_sample,2) data(idx_sample,2) 0]/255;    
                            elseif(data(idx_sample,2)>=256 && data(idx_sample,2)<512)
                                color = [0 512-data(idx_sample,2) data(idx_sample,2)-256]/255;    
                            elseif(data(idx_sample,2)>=512 && data(idx_sample,2)<640)
                                color = [data(idx_sample,2)-512 0 768-data(idx_sample,2)]/255;    
                            end
                            
                        end
                        
                     
                        
                       
                        
                        set(h{idx}, 'XData',data(1:idx_sample,3));
                        set(h{idx}, 'YData',data(1:idx_sample,4));
                        set(h{idx}, 'linewidth',1);
                        
                        
                        

                %         ht{1}= text(50,2.5,'red')
                %         ht{2}= text(130,2.5,'green')
                %         ht{3}= text(210,2.5,'blue')
                %         ht{4}= text(250,2.5,'violet')


                        hold on;
                        grid on;
                        ylim([0 5.1])

                        title('Spectrum scanning','fontsize',40);
                        xlabel('Sample count','fontsize',30)
                        ylabel('voltage','fontsize',30)

                            
%                         color = {'r',[0.75 0.25 0], [0.5 0.5 0],[0.25 0.75 0], 'g',[0 0.75 0.25],[0 0.5 0.5],[0 0.25 0.75],'b',[0.25 0 0.75],'m'}
%                         set(h{idx},'color','k');
%                         set(h{idx},'color','k');
                        
%                         if(newdata(2) ==0)
                        if(data(idx_sample,2) ==1)
                            
%                             ht{1}= text(20,2.7,'red');
                            ht{1}= text(data(idx_sample,2),1.5,'red');
                            set(ht{1},'fontsize',20, 'color','r');

                            %                         elseif(newdata(2) ==256)
                        elseif(data(idx_sample,2) ==256)
                            ht{2}= text(data(idx_sample,2),1.5,'green');
                            set(ht{2},'fontsize',20, 'color','g');

                            %                         elseif(newdata(2) ==512)
                        elseif(data(idx_sample,2) ==512)
                            ht{3}= text(data(idx_sample,2),1.5,'blue');
                            set(ht{3},'fontsize',20, 'color','b');
                        
%                         elseif(newdata(2) ==640)
                        elseif(data(idx_sample,2) ==640)
                            ht{4}= text(data(idx_sample,2),1.5,'violet');
                            set(ht{4},'fontsize',20, 'color','m');
                        end
                            
                        drawnow;
                        
                        if(i ==  sample_num)
                          idx_sample = 0;
                        end
                     end % end if
                        
            hold on


            grid on;

%              mode = mode + 1; % 0:same light, 1: scan
        end  % end of for
        
        %{
        if(data(end,1) < 4)
            if(mode ==1)
                legend_list =legend_str{4*idx-3};
            else
                legend_list =[legend_list {legend_str{4*idx-3}}];
            end
        end
        %}
        
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
%                             legend_list =legend_list; 
        hl = legend(legend_list);
        

        set(hl,'fontsize',20);
        set(hl,'location','southwest');

        if(idx ==5)
            set(h{idx},'linestyle',line_style{1});
            set(h{idx},'linewidth',3);
        elseif(idx ==6)
            set(h{idx},'linestyle',line_style{2});
            set(h{idx},'linewidth',3);
        end
        
                   
    end % end of if mode ~=0

    sprintf('idx = %d,mode = %d',idx,mode)

%        if(automode ~= 1)
%         reply = input('replace ready?','s');
%         while(reply~='y')   
%             disp('continue');
%         end
        
    hold on;
   

%     if(idx==5)
%         fprintf(s,'c');
%         break
%     end

    if(idx ==1)
        comp1 = data;
        before = data(:,4);
        smooth_data1 = smooth(data(:,4))
       
        
        reply = input('replace ready?','s');
        while(reply~='y')   
            disp('continue');
        end
    else
        comp2 = data;
        after = data(:,4);
        diff = before-after;
        smooth_data2 = smooth(data(:,4));
        
        
    end
    
    
    smooth_data = smooth(data(:,3))
    
    flushinput(s); 
     clear data;
    idx = idx +1;

end % end of for/while
fclose(s)
%%
figure(30)
%     plot(data(:,3),diff,'color','r', 'linestyle','-.')
plot(comp1(:,3),comp1(:,4),'color','k')    
hold on;
plot(comp2(:,3),comp2(:,4),'color','m')    
h_smooth1 = plot(comp1(:,3),smooth_data1,'color','r')    
h_smooth2 = plot(comp2(:,3),smooth_data2,'color','b')  

%%
hl_smooth1=legend(hl_smooth1,'plane water')
hl_smooth2=legend(hl_smooth2,'plane water')
%     hl_smooth2=legend([hl_smooth1 hl_smooth2],{'plane water','algae'})
set(hl_smooth1, 'fontsize',50)
set(hl_smooth2, 'fontsize',50)
set(hl_smooth1,'linewidth',3)
set(hl_smooth2,'linewidth',3)
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
figure(5)
plot(diff)
hold on;
plot(smooth(diff))

%% plot difference
figure(4)
%     plot(data(:,3),diff,'color','r', 'linestyle','-.')
plot(comp1(:,3),-diff,'color','k')    
hold on;
hp4 = plot(comp1(:,3),smooth(-diff),'color','r')    
set(hp4,'linewidth',3)
% ylim([-5 0 ])
axis auto
ht1= text(1,-1.5,'red');
ht2= text(256,-1.5,'green');
ht3= text(512,-1.5,'blue');
ht4= text(640,-1.5,'violet');
set(ht1,'color','r','fontsize',20);
set(ht2,'color','g','fontsize',20);
set(ht3,'color','b','fontsize',20);
set(ht4,'color','m','fontsize',20);
grid on;

title('Magnitude of spectrum difference','fontsize',30);
xlabel('Sample count','fontsize',30)
ylabel('voltage','fontsize',30)
% plot(comp1(:,3),smooth_data1,'color','m')    

 



      
%%

clear all
delete(instrfindall);

%%

fprintf(s,'c');
sample_num = num_data;
% sprintf('default mode')

%%

pl2 = figure(2);
pl3 = figure(3);

% lH = plot(rand(10,2));
% aH = ancestor(lH(1),'axes');
% figure(fH(2));
% aH(2) = axes;
% copyobj(lH(1),aH(2)); %copy line to axes
% copyobj(aH(1),fH(3)); %copy axes to figure


copyobj(pl2,pl3);

%% calculate optimal resistor
syms x
red_res = 2.8
blue_res = 5.7
v1 = red_res/(red_res+x)
v2 = blue_res/(blue_res+x)

diff_vol = diff(v2-v1,x)
% vpa(pretty(diff_vol),4)
pretty(diff_vol)

solve(diff_vol)
