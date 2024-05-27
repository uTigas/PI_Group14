import { IonButton, IonCol, IonContent, IonFooter, IonGrid, IonHeader, IonInput, IonItem, IonList, IonMenu, IonPage, IonRow, IonSplitPane, IonTitle, IonToolbar, IonAvatar, IonLabel, IonText, IonSegmentButton, IonIcon } from '@ionic/react';
import AppAppBar from '../components/AppAppBar';
import './Chats.css';
import { useContext, useEffect, useRef, useState } from 'react';
import { format } from 'date-fns';
import { UserContext } from '../App';
import ApiWrapper from '../support/APIWrapper';
import { checkmarkOutline, closeOutline, refresh } from 'ionicons/icons';

const Chats: React.FC = () => {
  const [activeChat, setActiveChat] = useState<any>(null);
  const [invited, setInvited] = useState<any>('');
  const [message, setMessage] = useState<any>('');
  const [contacts, setContacts] = useState<any[]>([]);
  const [messages, setMessages] = useState<any[]>([]);
  const [invites, setInvites] = useState<any[]>([]);
  const userDetails = useContext(UserContext);
  const [error, setError] = useState<boolean>(false);
  const inputRef = useRef<HTMLIonInputElement>(null)

  const inviteChat = async () => {
    if (invited != ''){
      setError(false);
      const formData = new FormData;
      formData.append('username', invited);
      const response = ApiWrapper.inviteChat(formData);
      if (await response){
        refresh()
        response.catch(
          (error) => {
            setError(true)
            console.log(error)
          }
        )
      }
      fetchContacts();
    }
    else{
      setError(true);   
    } 
  }

  const fetchContacts = () => {
    try {
      const response = ApiWrapper.fetchContacts();
      if (response) {
        response.then(
          (response) => {
            setContacts(response!.data.chats)
          }
        )
      } else {
        console.error('Response is undefined');
      }
    } catch (error) {
      console.error('Error fetching contacts:', error);
    }  }
  
  const fetchInvites = () => {
    try {
      const response = ApiWrapper.fetchChatInvites();
      if (response) {
        response.then(
          (response) => {
            setInvites(response!.data.invites)
          }
        )
      } else {
        console.error('Response is undefined');
      }
    } catch (error) {
      console.error('Error fetching invites:', error);
    }
  };
  
  const fetchChat = () => {
    try {
      const response = ApiWrapper.fetchMessages(activeChat.id);
      if (response) {
        response.then(
          (response) => {
            setMessages(response!.data.messages)
          }
        )
      } else {
        console.error('Response is undefined');
      }
    } catch (error) {
      console.error('Error fetching messages:', error);
    }
  }

  const acceptInvite = async (id:string) => {
    const formData = new FormData;
    formData.append('invite',id) 
    const response = ApiWrapper.acceptChatInvite(formData)
    if(await response)
      refresh()
  }
  
  const refuseInvite = async (id:string) => {
    const formData = new FormData;
    formData.append('invite',id) 
    const response = ApiWrapper.refuseChatInvite(formData)
    if(await response)
      refresh()
  }

  const sendMessage = async () => {
    const formData = new FormData;
    if (message != ''){
      formData.append('message', message) 
      formData.append('chat', activeChat.id)
      const response = ApiWrapper.sendMessage(formData)
      if(await response)
        refresh()
    }
  }
  
  const refresh = () => {
    setMessage('')
    fetchContacts()
    fetchInvites()
    if (activeChat)
      fetchChat()
  }

  useEffect(()=>{
    refresh()
  },[])
  
  useEffect(()=>{
    if (activeChat !== null) {
      fetchChat();
    }
  },[activeChat])

  return (
    <IonPage>
      <IonHeader>
        <AppAppBar title='Chats'/>
      </IonHeader>
      <IonContent>
        <IonSplitPane when='xs' contentId='main'>
          <IonMenu contentId='main'>
            <IonHeader>
              <IonInput className='ion-text-center' placeholder='Enter username...' onIonChange={(e) => setInvited(e.detail.value)}></IonInput>
              <IonSegmentButton onClick={() => {inviteChat()}}>Send Chat Invite</IonSegmentButton>
              {error ? (<IonText className='ion-text-center' color={'danger'}>FAILED! Invalid username or already sent this Invite.</IonText>):(<></>)}
            </IonHeader>
            <IonContent>
              <IonList>
                {invites.map((invite: any) => (
                  <IonItem key={invite.id}>
                    <IonAvatar slot="start">
                      <img src="https://ionicframework.com/docs/img/demos/avatar.svg" alt={`Avatar of ${invite.username}`} />
                    </IonAvatar>
                    <IonText><strong>{invite.inviter_name}</strong> wants to chat!</IonText>
                    <IonButton fill='outline' shape='round' color={'success'} onClick={() => acceptInvite(invite.id)}><IonIcon icon={checkmarkOutline}/></IonButton>
                    <IonButton fill='outline' shape='round' color={'danger'} onClick={() => refuseInvite(invite.id)}><IonIcon icon={closeOutline} /></IonButton>
                  </IonItem>
                ))}
                {contacts.map((contact: any) => (
                  <IonItem key={contact.id} button onClick={() => {setActiveChat(contact), setMessage('')}}>
                    <IonAvatar slot="start">
                      <img src="https://ionicframework.com/docs/img/demos/avatar.svg" alt={`Avatar of ${contact.name}`} />
                    </IonAvatar>
                    <strong>{contact.name}</strong>
                  </IonItem>
                ))}
              </IonList>
            </IonContent>
          </IonMenu>
          <div className='ion-page' id='main'>
            {activeChat ? (
              <><IonHeader>
                <IonToolbar>
                  <IonAvatar slot="start">
                      <img src="https://ionicframework.com/docs/img/demos/avatar.svg" className='ion-padding'/>
                    </IonAvatar>
                  <IonTitle>
                      {activeChat.name}
                  </IonTitle>
                </IonToolbar>
              </IonHeader><IonContent className='ion-padding'>
              <div className="ChatContainer">
                {messages.map((message: any) => (
                  <div 
                    key={message.id} 
                    className={`ChatBubble ${message.sender !== userDetails?.username ? 'received' : 'sent'}`}
                  >
                    <div className="MessageContent">
                      <strong>{message.message}</strong>
                      <IonText> {format(message.ts, "HH:MM")}</IonText>
                    </div>
                  </div>
                ))}
                </div>
                </IonContent><IonFooter>
                  <IonToolbar>
                    <IonGrid>
                      <IonRow>
                        <IonCol>
                          <IonInput ref={inputRef} clearInput={true} aria-label="text" placeholder='Enter text' onIonChange={(e) => setMessage(e.detail.value)}></IonInput>
                        </IonCol>
                        <IonCol size='auto' className='ion-text-end'>
                          <IonButton fill='outline' onClick={() => {sendMessage(), inputRef.current!.value=''}}>Send</IonButton>
                        </IonCol>
                      </IonRow>
                    </IonGrid>
                  </IonToolbar>
                </IonFooter></>
            ):(<ChatMessage />)}
          </div>
        </IonSplitPane>
      </IonContent>
    </IonPage>
  );
};

const ChatMessage: React.FC = () => {
  return (
    <IonContent>
      <div className="container">
        <strong>QeeP Quantum Secure Chat System</strong>
        <p>Only Chatters can access shared data. Your messages are quantum encrypted in order to maximize the privacy and safety of your most precious data.</p>
      </div>
    </IonContent>
  )
}

export default Chats;
