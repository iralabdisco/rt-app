# RT-APP Diagrams

## Overview

|         Supersedes          | Supereseded By |  Updated   |
| :-------------------------: | :------------: | :--------: |
| `diagramma_applicativo.png` |      N/A       | 2020-12-18 |

The diagram that follows is an overview of the components of RT-APP.

```mermaid
flowchart TD
	subgraph ONLINE ["Online (RT)"]
		%% DATA
		wheel_data>"WHEEL_DATA: DeviceDriver"]
		lidar_data>"LIDAR_DATA: DeviceDriver"]
		cmd_stream{{"MOV_CMD: OStream"}}

		subgraph ODOMETRY
		odo_freq([ODO_FREQ])
			odo_msgq[/"ODOMETRY: MessageQueue"/]

			odo_proc["ODOMETRY: Process"]

			odo_freq --> odo_proc
			odo_proc --> odo_msgq
		end
		wheel_data ----> odo_proc

		subgraph AMCL
			amcl_freq([LCM_FREQ])
			amcl_msgq[/"AMCL_POSE: MessageQueue"/]

			amcl_proc["COSTMAP: Process"]

			amcl_freq --> amcl_proc
			amcl_proc --> amcl_msgq
		end
		lidar_data ----> amcl_proc

		subgraph POSE_MGR
			pm_freq([PM_FREQ])
			pm_msgq[/POSE_QUEUE: MessageQueue/]

			pm_proc["POSE_MGR: Process"]

			pm_freq --> pm_proc
			odo_msgq ---> pm_proc
			amcl_msgq ---> pm_proc
			pm_proc --> pm_msgq
		end

		subgraph COSTMAP
			lcm_freq([LCM_FREQ])
			lcm_shm[("LCOSTMAP: Shmem/RW")]

			lcm_proc["COSTMAP: Process"]

			lcm_freq --> lcm_proc
			lcm_proc --> lcm_shm
		end
		lidar_data ----> lcm_proc

		subgraph LOC_PLAN
			lpl_freq([LPL_FREQ])

			lpl_proc[LOC_PLAN: Process]

			lpl_freq --> lpl_proc
			pm_msgq ---> lpl_proc
			lcm_shm ---> lpl_proc
		end

		lpl_proc --> cmd_stream

	end

	subgraph OFFLINE ["Offline"]
		subgraph GLOB_PLAN
			gpl_path[(GLOB_PATH: Shmem/WORM)]
			
			gpl_parms([PARMS])
			gpl_goal([GOAL])
			gpl_costmap(["GLOB_COSTMAP"])

			gpl_proc["GLOB_PLAN: Process"]
			
			gpl_parms --> gpl_proc
			gpl_goal --> gpl_proc
			gpl_costmap --> gpl_proc
			gpl_proc --> gpl_path

			gpl_path -....-> lpl_proc

		end
	end
```

## Legend

| Supersedes | Supereseded By |  Updated   |
| :--------: | :------------: | :--------: |
|    N/A     |      N/A       | 2020-12-18 |


```mermaid
flowchart TD
	subgraph LEGEND
		l_proc_1["Process"]
		l_proc_2["Merging Process"]
		
		l_input>"External Input"]
		l_data(["Data (struct)"])
		l_data_sub(["Subdata (member)"])
		l_ret_val(["Return Value"])
		
		l_input -- "Projected Timestamp" --> l_proc_1
		l_proc_1 -. "Leap (⨯iterations/+elapsed)" .-> l_data
		
		l_data --- l_data_sub
		l_data_sub ----> l_proc_2

		subgraph SUBROUTINE	
			ls_proc_1[["Black Box Process"]]
			ls_ret_val(["Subroutine Return Value"])

			ls_proc_1 --> ls_ret_val
		end

		l_data -. "TODO: alternative" ..-> ls_proc_1
		ls_ret_val --o l_proc_2
		l_proc_2 --> l_ret_val

		l_ret_val == "Loop (timing)" ==> l_proc_1
		
				
		l_proc_3["Producer Process"]
		l_proc_4["Consumer Process"]
		l_shm_att[("Shared Memory Attachment :: Name")]
		l_msg_en[/Sent Message :: Queue Name/]
		l_msg_deq[\Received Message :: Queue Name\]
		
		l_proc_3 ---> l_shm_att
		l_shm_att ---> l_proc_4
		l_msg_deq --> l_proc_4 
		l_proc_3 ----> l_msg_en

	end
```

## Flowchart

| Supersedes | Supereseded By |  Updated   |
| :--------: | :------------: | :--------: |
|    N/A     |      N/A       | 2020-12-18 |

The flowchart is not ready yet.

## Timing Chart

|              Supersedes              | Supereseded By |  Updated   |
| :----------------------------------: | :------------: | :--------: |
| `mermaid-diagram-20201217174645.svg` |      N/A       | 2020-12-18 |

The timing chart is a variation of the flowchart that tries to explain how the various components of RT-APP interact over time.

```mermaid
flowchart TD
	subgraph POSE_MGR
		subgraph POSE_MGR_ROUTINE_1 %% Subroutine 1: Update poses
			%% decl calc
			pm1_compose_0["compose"]
			pm1_compose_1["compose"]
			pm1_compose_2["compose"]
			%% decl data
			pm1_delta_pose_0[\"Δpose_180::odo_msgq"\]
			pm1_delta_pose_1[\"Δpose_200::odo_msgq"\]
			pm1_delta_pose_2[\"Δpose_2060::odo_msgq"\]
			pm1_pose_0("pose_160")
			pm1_pose_1("pose_180")
			pm1_pose_2("pose_2040")
			pm1_pose_3("pose_2060")
			pm1_final_pose("new_pose_2060")
			%% end decl

			pm1_delta_pose_0 --> pm1_compose_0
			pm1_pose_0 --> pm1_compose_0
			pm1_compose_0 --> pm1_pose_1
			
			pm1_delta_pose_1 --> pm1_compose_1
			pm1_pose_1 --> pm1_compose_1
			pm1_compose_1 -. +1840 .-> pm1_pose_2
			
			pm1_delta_pose_2 --> pm1_compose_2
			pm1_pose_2 --> pm1_compose_2
			pm1_compose_2 --> pm1_pose_3
		end

		subgraph POSE_MGR_ROUTINE_3
			%% decl calc
			pm3_select_accurate[select]
			%% decl data
			pm3_selected_pose_accurate("pose_172_accurate")
			%% end decl
			
			pm3_select_accurate -- ??? --> pm3_selected_pose_accurate
			pm1_pose_0 -. scegliere policy .-> pm3_select_accurate
		end

		subgraph POSE_MGR_ROUTINE_2 %% Subroutine 2: Interact with AMCL
			%% decl calc
			pm2_select_rough[select]
			pm2_select_decompose[select]
			pm2_compose["compose"]
			pm2_compute_pose_deltas["compute_Δpose(2063-172)"] 
			%% decl data
			pm2_selected_pose_rough("pose_172_rough")
			pm2_flat_pose_deltas("Δpose_(2063-172)")
			%% end decl

			pm2_select_rough -- 174 --> pm2_selected_pose_rough
			pm2_select_decompose -- 2064 --> pm2_compute_pose_deltas
		
			pm3_selected_pose_accurate -- 2064 --o pm2_compute_pose_deltas
			pm2_compute_pose_deltas -- 2065 --> pm2_flat_pose_deltas
            pm2_flat_pose_deltas --> pm2_compose
			
			pm1_pose_0 -. scegliere policy .-> pm2_select_rough
			pm1_pose_3 -. scegliere policy .-> pm2_select_decompose

			pm2_compose -- 2066 --> pm1_final_pose
		end 
	end %% POSE_MGR

	subgraph AMCL
		%% decl calc
		amcl_exec[["amcl_exec"]]
		%% decl data
		amcl_sens_data>"ldata_172"]
		amcl_sens_data_ts(["ldata_172_ts"])
		amcl_sens_data --- amcl_sens_data_ts 
		amcl_pose("amcl_pose_172")
		amcl_pose_ts("amcl_pose_172_ts")
		amcl_pose --- amcl_pose_ts
		%% end decl
		
		pm2_selected_pose_rough -- 175 --> amcl_exec
		amcl_sens_data -- 173 --> amcl_exec
		amcl_exec -- 2063 --> amcl_pose

		amcl_sens_data_ts -- 173 --> pm2_select_rough
		amcl_pose -- 2064 --o pm2_compose
		amcl_pose_ts -- 2064 --o pm2_select_decompose
	end %% AMCL

```

> ***NOTICE: The following diagrams are work-in-progress***

```mermaid
flowchart TD

	subgraph odo_1 ["ODOMETRY"]
		odo_1_exec_1["arc_to_rototr"]
		odo_1_exec_2["enqueue::odo_msgq"]

		odo_1_whl_data>"wdata_180"]
		odo_1_data(["rototr_180"])
		odo_1_msg[/"Δpose_180::odo_msgq"/]

		odo_1_whl_data -- 180 --> odo_1_exec_1
		odo_1_exec_1 -- 181 --> odo_1_data
		odo_1_data -- 181 --> odo_1_exec_2
		odo_1_exec_2 -- 181 --> odo_1_msg
	end

	subgraph odo_2 ["ODOMETRY"]
		odo_2_exec_1["arc_to_rototr"]
		odo_2_exec_2["enqueue::odo_msgq"]

		odo_2_whl_data>"wdata_200"]
		odo_2_data(["rototr_200"])
		odo_2_msg[/"Δpose_200::odo_msgq"/]

		odo_2_whl_data -- 200 --> odo_2_exec_1
		odo_2_exec_1 -- 201 --> odo_2_data
		odo_2_data -- 201 --> odo_2_exec_2
		odo_2_exec_2 -- 201 --> odo_2_msg
	end

	subgraph odo_3 ["ODOMETRY"]
		odo_3_exec_1["arc_to_rototr"]
		odo_3_exec_2["enqueue::odo_msgq"]

		odo_3_whl_data>"wdata_2060"]
		odo_3_data(["rototr_2060"])
		odo_3_msg[/"Δpose_2060::odo_msgq"/]

		odo_3_whl_data -- 2060 --> odo_3_exec_1
		odo_3_exec_1 -- 2061 --> odo_3_data
		odo_3_data -- 2061 --> odo_3_exec_2
		odo_3_exec_2 -- 2061 --> odo_3_msg
	end

	odo_1 -- 200 --> odo_2
	odo_2 -. +1860 .-> odo_3
	
	subgraph LOC_PLAN
		lp_glob_plan[("glob_plan")]
		LP_TODO
	end
```

<!-- EOF -->