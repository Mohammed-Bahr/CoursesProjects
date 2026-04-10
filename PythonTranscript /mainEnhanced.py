# """
# YouTube Transcript + Claude AI Summary
# --------------------------------------
# Fetches a YouTube transcript, summarizes it using Claude AI,
# and saves everything to a Markdown file.

# Requirements:
#     pip install youtube-transcript-api anthropic
# """

# import re
# import os
# import anthropic
# from youtube_transcript_api import YouTubeTranscriptApi, NoTranscriptFound, TranscriptsDisabled


# # ─────────────────────────────────────────────
# # 1. INPUT HELPERS
# # ─────────────────────────────────────────────

# def extract_video_id(user_input: str) -> str | None:
#     """Extract an 11-character video ID from a URL or plain ID string."""
#     patterns = [
#         r"(?:v=|\/shorts\/|youtu\.be\/)([0-9A-Za-z_-]{11})",
#     ]
#     for pattern in patterns:
#         match = re.search(pattern, user_input)
#         if match:
#             return match.group(1)
#     # Accept a raw 11-char ID directly
#     if re.fullmatch(r"[0-9A-Za-z_-]{11}", user_input.strip()):
#         return user_input.strip()
#     return None


# def get_user_input() -> tuple[str, str, bool]:
#     """
#     Prompt the user for:
#       - YouTube URL or video ID
#       - Preferred transcript language
#       - Whether to include timestamps
#     Returns (video_id, language_code, include_timestamps).
#     """
#     print("=" * 55)
#     print("   YouTube Transcript + Claude AI Summary")
#     print("=" * 55)

#     # --- Video ID ---
#     while True:
#         raw = input("\n📺 YouTube URL or Video ID: ").strip()
#         video_id = extract_video_id(raw)
#         if video_id:
#             print(f"   ✅ Video ID: {video_id}")
#             break
#         print("   ❌ Invalid URL or ID. Please try again.")

#     # --- Language ---
#     lang = input("\n🌐 Transcript language code [default: en]: ").strip() or "en"
#     print(f"   ✅ Language: {lang}")

#     # --- Timestamps ---
#     ts = input("\n⏱️  Include timestamps? (yes/no) [default: yes]: ").strip().lower()
#     include_timestamps = ts not in ("no", "n")

#     return video_id, lang, include_timestamps


# # ─────────────────────────────────────────────
# # 2. TRANSCRIPT FETCHING
# # ─────────────────────────────────────────────

# def list_available_languages(video_id: str) -> list[str]:
#     """Return a list of available language codes for the video."""
#     try:
#         transcript_list = YouTubeTranscriptApi().list(video_id)
#         return [t.language_code for t in transcript_list]
#     except Exception:
#         return []


# def fetch_transcript(video_id: str, language: str) -> list[dict]:
#     """
#     Fetch transcript lines for the given video and language.
#     Returns a list of dicts with 'text' and 'start' keys.
#     Raises an exception with a helpful message on failure.
#     """
#     api = YouTubeTranscriptApi()
#     try:
#         transcript = api.fetch(video_id, languages=[language])
#         return [{"text": line.text, "start": line.start} for line in transcript]
#     except NoTranscriptFound:
#         available = list_available_languages(video_id)
#         msg = f"No transcript found for language '{language}'."
#         if available:
#             msg += f" Available: {', '.join(available)}"
#         raise ValueError(msg)
#     except TranscriptsDisabled:
#         raise ValueError("Transcripts are disabled for this video.")
#     except Exception as e:
#         raise ValueError(f"Unexpected error fetching transcript: {e}")


# # ─────────────────────────────────────────────
# # 3. CLAUDE AI SUMMARY
# # ─────────────────────────────────────────────

# def summarize_with_claude(transcript_text: str) -> str:
#     """
#     Send the full transcript text to Claude and ask for a summary
#     with key points. Returns the AI-generated explanation string.
#     """
#     client = anthropic.Anthropic()  # reads ANTHROPIC_API_KEY from env

#     prompt = (
#         "You are a helpful assistant. Below is the transcript of a YouTube video.\n"
#         "Please provide:\n"
#         "1. A brief 2–3 sentence summary of what the video is about.\n"
#         "2. A bullet-point list of the key takeaways or main points.\n\n"
#         f"Transcript:\n{transcript_text}"
#     )

#     print("\n🤖 Sending transcript to Claude AI...")
#     message = client.messages.create(
#         model="claude-opus-4-5",
#         max_tokens=1024,
#         messages=[{"role": "user", "content": prompt}],
#     )

#     return message.content[0].text


# # ─────────────────────────────────────────────
# # 4. MARKDOWN FORMATTING
# # ─────────────────────────────────────────────

# def format_timestamp(seconds: float) -> str:
#     """Convert seconds to MM:SS format."""
#     minutes = int(seconds // 60)
#     secs = int(seconds % 60)
#     return f"{minutes:02d}:{secs:02d}"


# def build_markdown(video_id: str, ai_explanation: str,
#                    lines: list[dict], include_timestamps: bool) -> str:
#     """
#     Assemble the final Markdown document.

#     Format:
#         # Transcript for VIDEO_ID
#         ## AI Explanation
#         ## Transcript
#     """
#     md_lines = [f"# Transcript for {video_id}\n"]

#     # AI Explanation section
#     md_lines.append("## AI Explanation\n")
#     md_lines.append(ai_explanation.strip())
#     md_lines.append("\n")

#     # Transcript section
#     md_lines.append("## Transcript\n")
#     for line in lines:
#         if include_timestamps:
#             ts = format_timestamp(line["start"])
#             md_lines.append(f"- [{ts}] {line['text']}")
#         else:
#             md_lines.append(f"- {line['text']}")

#     return "\n".join(md_lines)


# # ─────────────────────────────────────────────
# # 5. FILE SAVING
# # ─────────────────────────────────────────────

# def save_markdown(content: str, video_id: str) -> str:
#     """
#     Save the Markdown content to a file.
#     Lets the user choose a filename or uses a default.
#     Returns the final filename used.
#     """
#     default_name = f"transcript_{video_id}.md"
#     filename_input = input(
#         f"\n💾 Output filename [default: {default_name}]: "
#     ).strip()
#     filename = filename_input if filename_input else default_name

#     # Ensure .md extension
#     if not filename.endswith(".md"):
#         filename += ".md"

#     with open(filename, "w", encoding="utf-8") as f:
#         f.write(content)

#     return filename


# # ─────────────────────────────────────────────
# # 6. MAIN
# # ─────────────────────────────────────────────

# def main():
#     # Step 1: Collect user preferences
#     video_id, language, include_timestamps = get_user_input()

#     # Step 2: Fetch transcript
#     print("\n⏳ Fetching transcript from YouTube...")
#     try:
#         lines = fetch_transcript(video_id, language)
#     except ValueError as e:
#         print(f"\n❌ {e}")
#         return

#     line_count = len(lines)
#     print(f"   ✅ {line_count} lines fetched.")

#     # Step 3: Build plain-text version for Claude
#     plain_text = "\n".join(line["text"] for line in lines)

#     # Step 4: Summarize with Claude (requires ANTHROPIC_API_KEY in env)
#     api_key = os.environ.get("ANTHROPIC_API_KEY")
#     if not api_key:
#         print("\n⚠️  ANTHROPIC_API_KEY not set. Skipping AI summary.")
#         ai_explanation = "_API key not provided — summary unavailable._"
#     else:
#         try:
#             ai_explanation = summarize_with_claude(plain_text)
#             print("   ✅ Summary received.")
#         except Exception as e:
#             print(f"\n⚠️  Claude API error: {e}")
#             ai_explanation = f"_Summary generation failed: {e}_"

#     # Step 5: Build Markdown document
#     markdown_content = build_markdown(video_id, ai_explanation, lines, include_timestamps)

#     # Step 6: Save to file
#     output_file = save_markdown(markdown_content, video_id)

#     # Done — show stats
#     size_kb = os.path.getsize(output_file) / 1024
#     print(f"\n✅ Done!")
#     print(f"   📄 File    : {output_file}")
#     print(f"   📝 Lines   : {line_count}")
#     print(f"   📦 Size    : {size_kb:.1f} KB")


# if __name__ == "__main__":
#     main()



"""
YouTube Transcript + Groq AI Summary
--------------------------------------
Fetches a YouTube transcript, summarizes it using Groq AI,
and saves everything to a Markdown file.

Requirements:
    pip install youtube-transcript-api groq
"""

import re
import os
from youtube_transcript_api import YouTubeTranscriptApi, NoTranscriptFound, TranscriptsDisabled
from groq import Groq

GROQ_API_KEY = os.environ.get("GROQ_API_KEY")
GROQ_MODEL = "llama-3.3-70b-versatile"
if not GROQ_API_KEY:
    raise ValueError("GROQ_API_KEY environment variable is not set")


# ─────────────────────────────────────────────
# 1. INPUT HELPERS
# ─────────────────────────────────────────────

def extract_video_id(user_input: str) -> str | None:
    """Extract an 11-character video ID from a URL or plain ID string."""
    patterns = [
        r"(?:v=|\/shorts\/|youtu\.be\/)([0-9A-Za-z_-]{11})",
    ]
    for pattern in patterns:
        match = re.search(pattern, user_input)
        if match:
            return match.group(1)
    # Accept a raw 11-char ID directly
    if re.fullmatch(r"[0-9A-Za-z_-]{11}", user_input.strip()):
        return user_input.strip()
    return None


def get_user_input() -> tuple[str, str, bool]:
    """
    Prompt the user for:
      - YouTube URL or video ID
      - Preferred transcript language
      - Whether to include timestamps
    Returns (video_id, language_code, include_timestamps).
    """
    print("=" * 55)
    print("   YouTube Transcript + Groq AI Summary")
    print("=" * 55)

    # --- Video ID ---
    while True:
        raw = input("\n📺 YouTube URL or Video ID: ").strip()
        video_id = extract_video_id(raw)
        if video_id:
            print(f"   ✅ Video ID: {video_id}")
            break
        print("   ❌ Invalid URL or ID. Please try again.")

    # --- Language ---
    lang = input("\n🌐 Transcript language code [default: en]: ").strip() or "en"
    print(f"   ✅ Language: {lang}")

    # --- Timestamps ---
    ts = input("\n⏱️  Include timestamps? (yes/no) [default: yes]: ").strip().lower()
    include_timestamps = ts not in ("no", "n")

    return video_id, lang, include_timestamps


# ─────────────────────────────────────────────
# 2. TRANSCRIPT FETCHING
# ─────────────────────────────────────────────

def list_available_languages(video_id: str) -> list[str]:
    """Return a list of available language codes for the video."""
    try:
        transcript_list = YouTubeTranscriptApi().list(video_id)
        return [t.language_code for t in transcript_list]
    except Exception:
        return []


def fetch_transcript(video_id: str, language: str) -> list[dict]:
    """
    Fetch transcript lines for the given video and language.
    Returns a list of dicts with 'text' and 'start' keys.
    Raises a ValueError with a helpful message on failure.
    """
    api = YouTubeTranscriptApi()
    try:
        transcript = api.fetch(video_id, languages=[language])
        return [{"text": line.text, "start": line.start} for line in transcript]
    except NoTranscriptFound:
        available = list_available_languages(video_id)
        msg = f"No transcript found for language '{language}'."
        if available:
            msg += f" Available: {', '.join(available)}"
        raise ValueError(msg)
    except TranscriptsDisabled:
        raise ValueError("Transcripts are disabled for this video.")
    except Exception as e:
        raise ValueError(f"Unexpected error fetching transcript: {e}")


# ─────────────────────────────────────────────
# 3. GROQ AI SUMMARY
# ─────────────────────────────────────────────

def summarize_with_groq(transcript_text: str) -> str:
    """
    Send the transcript to Groq and return a summary with key points.
    Uses the API key defined in GROQ_API_KEY at the top of this file.
    """
    client = Groq(api_key=GROQ_API_KEY)

    prompt = (
        "You are a helpful assistant. Below is the transcript of a YouTube video.\n"
        "Please provide:\n"
        "1. A brief 2–3 sentence summary of what the video is about.\n"
        "2. A bullet-point list of the key takeaways or main points.\n\n"
        f"Transcript:\n{transcript_text}"
    )

    print("\n🤖 Sending transcript to Groq AI...")
    response = client.chat.completions.create(
        model=GROQ_MODEL,
        messages=[{"role": "user", "content": prompt}],
        max_tokens=1024,
        temperature=0.5,
    )

    return response.choices[0].message.content


# ─────────────────────────────────────────────
# 4. MARKDOWN FORMATTING
# ─────────────────────────────────────────────

def format_timestamp(seconds: float) -> str:
    """Convert seconds (float) to MM:SS string."""
    minutes = int(seconds // 60)
    secs    = int(seconds % 60)
    return f"{minutes:02d}:{secs:02d}"


def build_markdown(video_id: str, ai_explanation: str,
                   lines: list[dict], include_timestamps: bool) -> str:
    """
    Assemble the final Markdown document in the required format:

        # Transcript for VIDEO_ID
        ## AI Explanation
        ## Transcript
    """
    md_lines = [f"# Transcript for {video_id}\n"]

    # AI Explanation section
    md_lines.append("## AI Explanation\n")
    md_lines.append(ai_explanation.strip())
    md_lines.append("\n")

    # Transcript section
    md_lines.append("## Transcript\n")
    for line in lines:
        if include_timestamps:
            ts = format_timestamp(line["start"])
            md_lines.append(f"- [{ts}] {line['text']}")
        else:
            md_lines.append(f"- {line['text']}")

    return "\n".join(md_lines)


# ─────────────────────────────────────────────
# 5. FILE SAVING
# ─────────────────────────────────────────────

def save_markdown(content: str, video_id: str) -> str:
    """
    Save the Markdown content to a file chosen by the user.
    Returns the final filename used.
    """
    default_name = f"transcript_{video_id}.md"
    filename_input = input(
        f"\n💾 Output filename [default: {default_name}]: "
    ).strip()
    filename = filename_input if filename_input else default_name

    # Ensure .md extension
    if not filename.endswith(".md"):
        filename += ".md"

    with open(filename, "w", encoding="utf-8") as f:
        f.write(content)

    return filename


# ─────────────────────────────────────────────
# 6. MAIN
# ─────────────────────────────────────────────

def main():
    # Step 1: Collect user preferences
    video_id, language, include_timestamps = get_user_input()

    # Step 2: Fetch transcript from YouTube
    print("\n⏳ Fetching transcript from YouTube...")
    try:
        lines = fetch_transcript(video_id, language)
    except ValueError as e:
        print(f"\n❌ {e}")
        return

    print(f"   ✅ {len(lines)} lines fetched.")

    # Step 3: Build plain text for Groq (trim to ~12 000 chars to stay within token limits)
    plain_text = "\n".join(line["text"] for line in lines)
    if len(plain_text) > 12_000:
        plain_text = plain_text[:12_000] + "\n... [transcript trimmed for length]"

    # Step 4: Summarize with Groq
    try:
        ai_explanation = summarize_with_groq(plain_text)
        print("   ✅ Summary received.")
    except Exception as e:
        print(f"\n⚠️  Groq API error: {e}")
        ai_explanation = f"_Summary generation failed: {e}_"

    # Step 5: Build Markdown document
    markdown_content = build_markdown(video_id, ai_explanation, lines, include_timestamps)

    # Step 6: Save to file
    output_file = save_markdown(markdown_content, video_id)

    # Done — show stats
    size_kb = os.path.getsize(output_file) / 1024
    print(f"\n✅ Done!")
    print(f"   📄 File    : {output_file}")
    print(f"   📝 Lines   : {len(lines)}")
    print(f"   📦 Size    : {size_kb:.1f} KB")


if __name__ == "__main__":
    main()