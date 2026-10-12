# Evidence to Capture for Activity 2 (like the reference report)

The reference PDF (Devesh Patil's submission) contains multiple **screenshots of the GitHub Issues** page — these serve as proof the QA workflow really happened. You should do the same, since your classmate will have similar evidence.

## 1) Create the 5 issues on github.com

Open: **https://github.com/RavirajSonar40/PM-ACTIVITY/issues**

Click **New issue**, then for each of the 5 issues below:
- **Title** = the issue title below
- **Body** = paste the full markdown from `evidence/issue-1.md` ... `evidence/issue-5.md` (each file contains the ready-made body with 5-Why analysis, reproduction steps, and resolution log)
- **Labels** = add `bug` for #1-#4, `enhancement` for #5. Use the severity noted inside each file as the issue-level tag too (`high` / `medium` / `low`) - create these labels first via Issues → Labels.

| Issue | Title |
|---|---|
| #1 | Blocking delay(200) in the main loop stalls LCD and servo |
| #2 | Barrier/LED stutter at distance threshold due to missing hysteresis |
| #3 | PIR false triggers from stray thermal pulses (no debounce) |
| #4 | Ultrasonic echo timeout freezes the state machine |
| #5 | LCD backlight stays on permanently, wasting power |

**Do not close the issues manually.** They should close automatically when you merge the fix branches through Pull Requests whose body says `Closes #N`, **OR** (if you already pushed to `main` like we did) close each one manually with a comment "Resolved by commit <hash>".

⚠️ Important note: our repo pushed the fix commits to `main` directly. GitHub only **auto-closes an issue** when the `fixes #N` keyword commit lands via a **Pull Request** (or merge to the default branch via the GitHub web UI), not on a local `git push origin main`. So:
- For full auto-close workflow: **repo Settings → check "Automatically close issues via commit messages" is enabled** (default on GitHub.com for merges through PR)
- Or simply close each issue manually with a one-line comment referencing the commit.

## 2) Open PRs (optional but matches the reference)

Go to the repo's **Pull requests → New pull request** and open one PR per branch:
- base: `main` ← compare: `fix/issue-2-hysteresis` (body: "Closes #2")
- base: `main` ← compare: `fix/issue-3-pir-debounce` (body: "Closes #3")
- ... and so on for #1-#5.

Even though the code is already merged into `main`, GitHub will show these as "already merged" PRs and still link their references to the issues — good for the collaboration evidence.

## 3) Screenshots to take (paste into your report)

Capture screenshots similar to the reference PDF. At minimum take these 6:

1. **Issues tab overview** — full list of 5 issues with labels and closed/open status (URL: `/issues`)
2. **One detailed issue page** — e.g., `/issues/1` showing title, 5-Why body, and labels
3. **Issue comment thread** — a comment on one issue quoting the fix commit
4. **Network / commit graph** — `Insights → Network` showing the 5 fix branches and merge bubbles
5. **Commit traceability** — either a screenshot of a commit page showing "fixes #N" text with the auto-linked `#N` in blue, OR the commit list with hash rows visible
6. **PR list** (if you do step 2) — showing 5 PRs, referencing issues

Tip: keep the browser at 100% zoom and crop the screenshots to remove your bookmarks bar. The reference report inserts each screenshot right after the section it proves.

## 4) Where to place each screenshot in the report

| Report section | Screenshot |
|---|---|
| A. GitHub Repository Link | Repo landing page (`/`) |
| C. QA Issues Logged (intro) | Issue list overview (#1) |
| C.1 Deep-dive #1 | Issue #1 page (`/issues/1`) |
| C.2 Deep-dive #2 | Issue #2 page (`/issues/2`) |
| C.3 Deep-dive #3 | Issue #3 page (`/issues/3`) |
| C.4 Deep-dive #4 | Issue #4 page (`/issues/4`) |
| C.5 Deep-dive #5 | Issue #5 page (`/issues/5`) |
| D. Collaboration Summary | PR list + one issue comment thread |
| G.2 Git Commit Traceability Matrix | Commit graph / Insights → Network |

## 5) committing the evidence folder (optional)

Commit the evidence screenshot PNGs into the repo itself so reviewers can see them:

```bash
cd gate-barrier-system
mkdir -p evidence/screenshots
# put the PNG files here, named 01-issue-list.png, 02-issue-1.png, ...
git add evidence/screenshots
git commit -m "add QA evidence screenshots"
git push
```

This mirrors how real industrial QA teams archive testing artifacts alongside source.
