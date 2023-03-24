#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <cstring>
#include <algorithm>

int duration_time;
int problem_count;
int temp_rank;
int problem_num;
int i;
std::string name, status, last_state, temp;
char problem_id;
bool start_check = false;
bool freeze_check = false;
bool scroll_check = false;
char temp_2;
char problem_name;
int status_id;
std::string temp_1;
struct Team {
  std::string team_name;
  int question_status[30] = {0};// 1 means 1Y, 0 means untouched
  int time_penalty[30] = {0};// time penalty of each problem
  int sum_time = 0;// sum of time penalty
  std::vector<int> time_sort;// the sorted time penalty
  std::set<int> freeze_id;// the sorted id of the frozen problems
  int rank;// the rank of each team
  int last_status[30][5];// the last time when a submission is of problem ? and status ?
  // if we have a first-zero, it means that the question is ALL
  // while the second zero means that the status is ALL
  int problem_last_status[30];// the last status when a submission is of problem ?
  int status_last_problem[5];// the last problem when a submission is of status ?
  int all_last_problem;// the last submitted question
  int all_last_status;// the status of the last submission
  int after_freeze[30] = {0};// the number of submission after FREEZE
  bool freeze_state[30];// whether problem ? is frozen
  int accepted_time[30] = {0};// when problem ? is accepted

  friend bool operator<(const Team &team_1, const Team &team_2) {
    //the comparison rule
    if (team_1.time_sort.size() > team_2.time_sort.size()) return true;
    if (team_1.time_sort.size() < team_2.time_sort.size()) return false;
    if (team_1.sum_time < team_2.sum_time) return true;
    if (team_1.sum_time > team_2.sum_time) return false;
    for (i = (int) team_1.time_sort.size() - 1; i >= 0; --i) {
      if (team_1.time_sort[i] < team_2.time_sort[i]) return true;
      if (team_1.time_sort[i] > team_2.time_sort[i]) return false;
    }
    if (team_1.team_name < team_2.team_name) return true;
    return false;
  }
};

std::unordered_map<std::string, Team> team_map;
std::unordered_map<std::string, Team> unfrozen_map;
std::set<Team> team_set;

inline void AddTeam() {
  // add a team
  if (start_check) {// if we have started, teams can't be added
    std::cout << "[Error]Add failed: competition has started." << '\n';
    return;
  }
  std::cin >> name;
  auto add_check = team_map.find(name);
  if (add_check != team_map.end()) {
    // whether such team has existed
    std::cout << "[Error]Add failed: duplicated team name." << '\n';
    return;
  }
  std::cout << "[Info]Add successfully." << '\n';
  team_map[name].team_name = name;
}

inline void Start() {
  if (start_check) {
    // we can't start a contest twice
    std::cout << "[Error]Start failed: competition has started." << '\n';
    return;
  }
  std::cin >> temp >> duration_time >> temp >> problem_count;
  start_check = true;
  std::cout << "[Info]Competition starts." << '\n';
  temp_rank = 1;
  for (const auto &insert_team : team_map) {
    // sort the teams in alphabetical order
    team_set.insert(insert_team.second);
  }
  for (auto  iter = team_set.begin(); iter != team_set.end(); ++iter) {
    team_map[iter->team_name].rank = temp_rank;
    ++temp_rank;// save the rank
  }
}

inline void Submit() {
  int time;
  std::cin >> problem_id;
  problem_num = problem_id - 'A' + 1;
  // convert the problem's name into integer
  std::cin >> temp >> name >> temp >> status >> temp >> time;

  Team &team_edit = team_map[name];
  Team &team_actual = unfrozen_map[name];
  if (!freeze_check) {
    // no freeze, save the information into the set
    Team old_team = team_map.find(name)->second;
    team_set.erase(old_team);
    if (status == "Accepted") {
      if (team_edit.question_status[problem_num] <= 0) {
        // submission after the "accepted" status isn't considered
        team_edit.sum_time += time + 20 * (-team_edit.question_status[problem_num]);
        // add penalty time
        team_edit.question_status[problem_num]
            = abs(team_edit.question_status[problem_num]) + 1;
        // change the question_status
        team_edit.time_sort.push_back(time);
        // another question is AC
        team_edit.time_penalty[problem_num]
            = time + 20 * (-team_edit.question_status[problem_num]);
        // update penalty time
        team_edit.accepted_time[problem_num] = time;
        // update accepted time
      }
      // update the necessary information
      team_edit.last_status[problem_num][1] = time;
      team_edit.last_status[problem_num][0] = time;
      team_edit.last_status[0][1] = time;
      team_edit.last_status[0][0] = time;
      team_edit.problem_last_status[problem_num] = 1;
      team_edit.status_last_problem[1] = problem_num;
      team_edit.all_last_problem = problem_num;
      team_edit.all_last_status = 1;
    } else {
      if (team_edit.question_status[problem_num] <= 0)
        --team_edit.question_status[problem_num];
      if (status == "Wrong_Answer") {
        team_edit.last_status[problem_num][2] = time;
        team_edit.last_status[problem_num][0] = time;
        team_edit.last_status[0][2] = time;
        team_edit.last_status[0][0] = time;
        team_edit.problem_last_status[problem_num] = 2;
        team_edit.status_last_problem[2] = problem_num;
        team_edit.all_last_problem = problem_num;
        team_edit.all_last_status = 2;
      }
      if (status == "Runtime_Error") {
        team_edit.last_status[problem_num][3] = time;
        team_edit.last_status[problem_num][0] = time;
        team_edit.last_status[0][3] = time;
        team_edit.last_status[0][0] = time;
        team_edit.problem_last_status[problem_num] = 3;
        team_edit.status_last_problem[3] = problem_num;
        team_edit.all_last_problem = problem_num;
        team_edit.all_last_status = 3;
      }
      if (status == "Time_Limit_Exceed") {
        team_edit.last_status[problem_num][4] = time;
        team_edit.last_status[problem_num][0] = time;
        team_edit.last_status[0][4] = time;
        team_edit.last_status[0][0] = time;
        team_edit.problem_last_status[problem_num] = 4;
        team_edit.status_last_problem[4] = problem_num;
        team_edit.all_last_problem = problem_num;
        team_edit.all_last_status = 4;
      }
    }
    Team new_team = team_map.find(name)->second;
    team_set.insert(new_team);
  } else {
    // we need to use another unordered_map to store the updated information
    if (team_edit.question_status[problem_num] <= 0) {
      ++team_actual.after_freeze[problem_num];
      team_edit.freeze_id.insert(problem_num);
      team_edit.freeze_state[problem_num] = true;
    }
    if (status == "Accepted") {
      if (team_actual.question_status[problem_num] <= 0) {
        team_actual.sum_time
            += time + 20 * (-team_actual.question_status[problem_num]);
        team_actual.question_status[problem_num]
            = 1 + abs(team_actual.question_status[problem_num]);
        team_actual.time_sort.push_back(time);
        team_actual.time_penalty[problem_num] =
            time + 20 * (team_actual.question_status[problem_num] - 1);
        team_actual.accepted_time[problem_num] = time;
      }
      team_actual.last_status[problem_num][1] = time;
      team_actual.last_status[problem_num][0] = time;
      team_actual.last_status[0][1] = time;
      team_actual.last_status[0][0] = time;
      team_actual.problem_last_status[problem_num] = 1;
      team_actual.status_last_problem[1] = problem_num;
      team_actual.all_last_problem = problem_num;
      team_actual.all_last_status = 1;
    } else {
      if (team_actual.question_status[problem_num] <= 0)
        --team_actual.question_status[problem_num];
      if (status == "Wrong_Answer") {
        team_actual.last_status[problem_num][2] = time;
        team_actual.last_status[problem_num][0] = time;
        team_actual.last_status[0][2] = time;
        team_actual.last_status[0][0] = time;
        team_actual.problem_last_status[problem_num] = 2;
        team_actual.status_last_problem[2] = problem_num;
        team_actual.all_last_problem = problem_num;
        team_actual.all_last_status = 2;
      }
      if (status == "Runtime_Error") {
        team_actual.last_status[problem_num][3] = time;
        team_actual.last_status[problem_num][0] = time;
        team_actual.last_status[0][3] = time;
        team_actual.last_status[0][0] = time;
        team_actual.problem_last_status[problem_num] = 3;
        team_actual.status_last_problem[3] = problem_num;
        team_actual.all_last_problem = problem_num;
        team_actual.all_last_status = 3;
      }
      if (status == "Time_Limit_Exceed") {
        team_actual.last_status[problem_num][4] = time;
        team_actual.last_status[problem_num][0] = time;
        team_actual.last_status[0][4] = time;
        team_actual.last_status[0][0] = time;
        team_actual.problem_last_status[problem_num] = 4;
        team_actual.status_last_problem[4] = problem_num;
        team_actual.all_last_problem = problem_num;
        team_actual.all_last_status = 4;
      }
    }
  }
}

inline void Flush() {
  // updating the rank
  std::cout << "[Info]Flush scoreboard." << '\n';
  temp_rank = 1;
  for (auto iter = team_set.begin(); iter != team_set.end(); ++iter) {
    // updating the rank one by one
    team_map[iter->team_name].rank = temp_rank;
    ++temp_rank;
  }
}

inline void Query_Rank() {
  std::cin >> name;
  auto iter = team_map.find(name);
  if (iter == team_map.end()) {
    // can't find such team
    std::cout << "[Error]Query ranking failed: cannot find the team."
              << '\n';
    return;
  }
  std::cout << "[Info]Complete query ranking." << '\n';
  if (freeze_check)
    std::cout << "[Warning]Scoreboard is frozen. The ranking may be inaccurate until it were scrolled."
              << '\n';
  std::cout << name << " NOW AT RANKING " << iter->second.rank << '\n';
}

inline void Query_Submission() {
  std::cin >> name;
  std::cin >> temp_1;
  while ((temp_2 = std::cin.get()) != '=') {}// an empty loop
  std::cin >> problem_name;
  temp_2 = std::cin.get();// redundant messages
  if (temp_2 == 'L')
    problem_id = 0; // which means the problem_id is "ALL"
  else problem_id = problem_name - 'A' + 1;
  std::cin.get();
  std::cin >> temp_1;
  while ((temp_2 = std::cin.get()) != '=') {}
  std::cin >> status;
  if (status == "ALL") status_id = 0;
  if (status == "Accepted") status_id = 1;
  if (status == "Wrong_Answer") status_id = 2;
  if (status == "Runtime_Error") status_id = 3;
  if (status == "Time_Limit_Exceed") status_id = 4;
  // convert the status into integers
  if (freeze_check == false) {
    // no FREEZE! the true information is stored in the unordered_map
    auto query_check = team_map.find(name);
    if (query_check == team_map.end()) {
      std::cout << "[Error]Query submission failed: cannot find the team."
                << '\n';
      return;
    }
    Team &query = team_map[name];
    std::cout << "[Info]Complete query submission." << '\n';
    if (query.last_status[problem_id][status_id] == 0) {
      std::cout << "Cannot find any submission." << '\n';
      return;
    }
    if (problem_id == 0 && status_id == 0) {
      switch (query.all_last_status) {
        case 1:last_state = "Accepted";
          break;
        case 2:last_state = "Wrong_Answer";
          break;
        case 3:last_state = "Runtime_Error";
          break;
        case 4:last_state = "Time_Limit_Exceed";
          break;
      }
      std::cout << name << " " << (char) (query.all_last_problem + 'A' - 1)
                << " " << last_state << " "
                << query.last_status[problem_id][status_id] << '\n';
    } else if (problem_id == 0) {
      std::cout << name << " "
                << (char) (query.status_last_problem[status_id] + 'A' - 1)
                << " " << status << " "
                << query.last_status[problem_id][status_id] << '\n';
    } else if (status_id == 0) {
      switch (query.problem_last_status[problem_id]) {
        case 1:last_state = "Accepted";
          break;
        case 2:last_state = "Wrong_Answer";
          break;
        case 3:last_state = "Runtime_Error";
          break;
        case 4:last_state = "Time_Limit_Exceed";
          break;
      }
      std::cout << name << " " << problem_name << " " << last_state << " "
                << query.last_status[problem_id][status_id]
                << '\n';
    } else {
      std::cout << name << " " << problem_name << " " << status << " "
                << query.last_status[problem_id][status_id]
                << '\n';
    }
  } else {
    // FREEZE! the information is stored in the unfrozen_map
    auto query_check = unfrozen_map.find(name);
    if (query_check == unfrozen_map.end()) {
      std::cout << "[Error]Query submission failed: cannot find the team."
                << '\n';
      return;
    }
    Team actual_query = unfrozen_map[name];
    std::cout << "[Info]Complete query submission." << '\n';
    if (actual_query.last_status[problem_id][status_id] == 0) {
      std::cout << "Cannot find any submission." << '\n';
      return;
    }
    if (problem_id == 0 && status_id == 0) {
      switch (actual_query.all_last_status) {
        case 1:last_state = "Accepted";
          break;
        case 2:last_state = "Wrong_Answer";
          break;
        case 3:last_state = "Runtime_Error";
          break;
        case 4:last_state = "Time_Limit_Exceed";
          break;
      }
      std::cout << name << " "
                << (char) (actual_query.all_last_problem + 'A' - 1)
                << " " << last_state << " "
                << actual_query.last_status[problem_id][status_id] << '\n';
    } else if (problem_id == 0) {
      std::cout << name << " "
                << (char) (actual_query.status_last_problem[status_id] + 'A' - 1)
                << " " << status << " "
                << actual_query.last_status[problem_id][status_id] << '\n';
    } else if (status_id == 0) {
      switch (actual_query.problem_last_status[problem_id]) {
        case 1:last_state = "Accepted";
          break;
        case 2:last_state = "Wrong_Answer";
          break;
        case 3:last_state = "Runtime_Error";
          break;
        case 4:last_state = "Time_Limit_Exceed";
          break;
      }
      std::cout << name << " " << problem_name << " " << last_state << " "
                << actual_query.last_status[problem_id][status_id]
                << '\n';
    } else {
      std::cout << name << " " << problem_name << " " << status << " "
                << actual_query.last_status[problem_id][status_id]
                << '\n';
    }
  }
}

inline void Freeze() {
  if (freeze_check && !scroll_check) {
    std::cout << "[Error]Freeze failed: scoreboard has been frozen." << '\n';
    return;
  }
  freeze_check = true;
  std::cout << "[Info]Freeze scoreboard." << '\n';
  // the state of submission_after_freeze should be updated to 0!
  for (auto iter = team_map.begin(); iter != team_map.end(); ++iter) {
    memset(iter->second.after_freeze, 0, sizeof(iter->second.after_freeze));
    unfrozen_map[iter->first] = iter->second;
  }
  scroll_check = false;

}

inline void Scroll() {
  if (!freeze_check) {
    std::cout << "[Error]Scroll failed: scoreboard has not been frozen." << '\n';
    return;
  }
  std::cout << "[Info]Scroll scoreboard." << '\n';
  //first of all, flush and print the list
  team_set.clear();
  temp_rank = 1;
  for (const auto &iter : team_map) {
    team_set.insert(iter.second);
  }
  for (auto iter = team_set.begin(); iter != team_set.end(); ++iter) {
    team_map[iter->team_name].rank = temp_rank;
    ++temp_rank;
  }
  for (auto iter = team_set.begin(); iter != team_set.end(); ++iter) {
    auto before_team = team_map[iter->team_name];
    auto check_iter = unfrozen_map[iter->team_name];
    std::cout << iter->team_name << " " << before_team.rank
              << " " << iter->time_sort.size() << " " << iter->sum_time << " ";
    for (i = 1; i <= problem_count; ++i) {
      if (iter->freeze_state[i]) {
        // the question has been frozen
        std::cout << before_team.question_status[i] << "/"
                  << check_iter.after_freeze[i] << " ";
      } else if (before_team.question_status[i] == 1) {
        std::cout << "+ ";
      } else if (before_team.question_status[i] > 1) {
        std::cout << "+" << before_team.question_status[i] - 1 << " ";
      } else if (before_team.question_status[i] == 0) {
        std::cout << ". ";
      } else {
        std::cout << before_team.question_status[i] << " ";
      }
    }
    std::cout << '\n';
  }
  // begin the real scroll process
  auto last_team = team_set.end();
  --last_team;// find the last team
  while (last_team != team_set.begin()) {
    auto original_next = ++last_team;
    // the original next team
    --last_team;
    if (last_team->freeze_id.empty()) {
      // this team has no frozen question! consider the one in front of it
      --last_team;
      continue;
    }
    auto delete_iter = last_team;
    name = last_team->team_name;
    Team &insert_iter = team_map[last_team->team_name];
    const auto &new_iter = unfrozen_map[last_team->team_name];
    const auto &unfreeze_id = last_team->freeze_id.begin();
    insert_iter.question_status[*unfreeze_id]
        = new_iter.question_status[*unfreeze_id];
    if (insert_iter.question_status[*unfreeze_id] > 0) {
      insert_iter.time_sort.push_back(new_iter.accepted_time[*unfreeze_id]);
      sort(insert_iter.time_sort.begin(), insert_iter.time_sort.end());
      insert_iter.sum_time += new_iter.time_penalty[*unfreeze_id];
    }
    // update the latest information
    insert_iter.freeze_id.erase(*unfreeze_id);
    team_set.erase(delete_iter);
    insert_iter = team_map[name];
    auto new_next = ++team_set.insert(insert_iter).first;
    if (new_next != original_next) {
      // compare their address, if not identical, the scroll process works
      std::cout << insert_iter.team_name << " " << new_next->team_name << " "
                << insert_iter.time_sort.size() << " "
                << insert_iter.sum_time << '\n';
    }
    --original_next;
    last_team = original_next;
    if (last_team->freeze_id.empty()) {
      // this team have no frozen question, consider the one in front of it
      --last_team;
    }
  }
  // that's the end of the scroll process
  team_set.clear();
  // update the latest information and print again
  temp_rank = 1;
  for (const auto &iter : unfrozen_map) {
    team_set.insert(iter.second);
  }
  for (auto iter = team_set.begin(); iter != team_set.end(); ++iter) {
    unfrozen_map[iter->team_name].rank = temp_rank;
    ++temp_rank;
  }
  for (auto iter = team_set.begin(); iter != team_set.end(); ++iter) {
    auto &after_team = unfrozen_map[iter->team_name];
    std::cout << iter->team_name << " " << after_team.rank
              << " " << iter->time_sort.size() << " "
              << iter->sum_time << " ";
    for (i = 1; i <= problem_count; ++i) {
      if (after_team.question_status[i] == 1) {
        std::cout << "+ ";
      } else if (after_team.question_status[i] > 1) {
        std::cout << "+" << after_team.question_status[i] - 1 << " ";
      } else if (after_team.question_status[i] == 0) {
        std::cout << ". ";
      } else {
        std::cout << after_team.question_status[i] << " ";
      }
    }
    std::cout << '\n';
  }
  team_map.clear();
  for (auto iter = unfrozen_map.begin(); iter != unfrozen_map.end(); ++iter)
    team_map[iter->first] = iter->second;
  freeze_check = false;
  scroll_check = true;
}

std::string operation;

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
   //freopen("bigger.in", "r", stdin);
   //freopen("i am big.out", "w", stdout);
  while (true) {
    std::cin >> operation;
    if (operation == "END") {
      std::cout << "[Info]Competition ends." << '\n';
      //fclose(stdin);
      //fclose(stdout);
      return 0;
    }
    if (operation == "ADDTEAM") AddTeam();
    if (operation == "START") Start();
    if (operation == "SUBMIT") Submit();
    if (operation == "FLUSH") Flush();
    if (operation == "QUERY_RANKING") Query_Rank();
    if (operation == "QUERY_SUBMISSION") Query_Submission();
    if (operation == "FREEZE") Freeze();
    if (operation == "SCROLL") Scroll();
  }
}